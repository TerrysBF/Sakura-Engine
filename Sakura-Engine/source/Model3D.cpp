#include "Model3D.h"
#include "OBJReader.h"
#include "MeshComponent.h"

#include <fbxsdk.h>
#include <string>

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

// Revisa si la ruta termina en .fbx / .FBX
static bool EndsWithFBX(const std::string& s)
{
  if (s.size() < 4) return false;
  std::string ext = s.substr(s.size() - 4);
  return (ext == ".fbx" || ext == ".FBX");
}

// Firmas de helpers internos solo visibles en este .cpp
static bool LoadFBXIntoMeshes(const std::string& basePath, std::vector<MeshComponent>& outMeshes);
static void ProcessFBXNode(FbxNode* node, std::vector<MeshComponent>& outMeshes);
static void ProcessFBXMesh(FbxNode* node, std::vector<MeshComponent>& outMeshes);

// -----------------------------------------------------------------------------
// FBX: carga escena y recorre nodos
// -----------------------------------------------------------------------------

static bool LoadFBXIntoMeshes(const std::string& basePath, std::vector<MeshComponent>& outMeshes)
{
  outMeshes.clear();

  // Aseguramos que el path termina en .fbx
  std::string filePath = basePath;
  if (!EndsWithFBX(filePath))
    filePath += ".fbx";

  // Manager principal del SDK
  FbxManager* sdkManager = FbxManager::Create();
  if (!sdkManager)
    return false;

  // IO settings
  FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
  sdkManager->SetIOSettings(ios);

  // Importador
  FbxImporter* importer = FbxImporter::Create(sdkManager, "");
  if (!importer->Initialize(filePath.c_str(), -1, sdkManager->GetIOSettings()))
  {
    importer->Destroy();
    sdkManager->Destroy();
    return false;
  }

  // Escena
  FbxScene* scene = FbxScene::Create(sdkManager, "scene");
  if (!importer->Import(scene))
  {
    importer->Destroy();
    scene->Destroy();
    sdkManager->Destroy();
    return false;
  }

  importer->Destroy();

  // Triangular toda la geometría
  FbxGeometryConverter converter(sdkManager);
  converter.Triangulate(scene, /*replace*/ true);

  // Root
  FbxNode* root = scene->GetRootNode();
  if (!root)
  {
    scene->Destroy();
    sdkManager->Destroy();
    return false;
  }

  // Recorremos recursivamente toda la jerarquía
  ProcessFBXNode(root, outMeshes);

  bool hasMeshes = !outMeshes.empty();

  scene->Destroy();
  sdkManager->Destroy();

  return hasMeshes;
}

static void ProcessFBXNode(FbxNode* node, std::vector<MeshComponent>& outMeshes)
{
  if (!node) return;

  FbxNodeAttribute* attr = node->GetNodeAttribute();
  if (attr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
  {
    ProcessFBXMesh(node, outMeshes);
  }

  const int childCount = node->GetChildCount();
  for (int i = 0; i < childCount; ++i)
  {
    ProcessFBXNode(node->GetChild(i), outMeshes);
  }
}

// -----------------------------------------------------------------------------
// FBX: convierte un FbxMesh en MeshComponent (posición + UV bien mapeados)
// -----------------------------------------------------------------------------

static void ProcessFBXMesh(FbxNode* node, std::vector<MeshComponent>& outMeshes)
{
  FbxMesh* mesh = node->GetMesh();
  if (!mesh) return;

  // Normales/Tangentes por si las necesitas después (ahorita no las guardamos)
  if (mesh->GetElementNormalCount() == 0)
    mesh->GenerateNormals(true, true);

  const char* uvSetName = nullptr;
  {
    FbxStringList uvSets;
    mesh->GetUVSetNames(uvSets);
    if (uvSets.GetCount() > 0)
      uvSetName = uvSets[0];
  }

  if (mesh->GetElementTangentCount() == 0 && uvSetName)
    mesh->GenerateTangentsData(uvSetName);

  const FbxGeometryElementUV* uvElem =
    (mesh->GetElementUVCount() > 0) ? mesh->GetElementUV(0) : nullptr;

  std::vector<SimpleVertex>   vertices;
  std::vector<unsigned int>   indices;
  vertices.reserve(mesh->GetPolygonCount() * 3);
  indices.reserve(mesh->GetPolygonCount() * 3);

  // Helper para leer UV correcto sin importar el tipo de mapeo
  auto readV2 = [](const FbxGeometryElementUV* elem, int cpIdx, int pvIdx) -> FbxVector2
    {
      if (!elem) return FbxVector2(0, 0);
      using E = FbxGeometryElement;
      int idx;
      if (elem->GetMappingMode() == E::eByControlPoint)
        idx = (elem->GetReferenceMode() == E::eIndexToDirect)
        ? elem->GetIndexArray().GetAt(cpIdx)
        : cpIdx;
      else
        idx = (elem->GetReferenceMode() == E::eIndexToDirect)
        ? elem->GetIndexArray().GetAt(pvIdx)
        : pvIdx;
      return elem->GetDirectArray().GetAt(idx);
    };

  const int polyCount = mesh->GetPolygonCount();
  for (int p = 0; p < polyCount; ++p)
  {
    const int polySize = mesh->GetPolygonSize(p);
    std::vector<unsigned> cornerIdx;
    cornerIdx.reserve(polySize);

    for (int v = 0; v < polySize; ++v)
    {
      const int cpIndex = mesh->GetPolygonVertex(p, v);
      const int pvIndex = mesh->GetPolygonVertexIndex(p) + v;

      SimpleVertex out{};

      // Posición local
      FbxVector4 P = mesh->GetControlPointAt(cpIndex);
      out.Pos = {
          (float)P[0],
          (float)P[1],
          (float)P[2]
      };

      // UV (V invertida para DirectX)
      if (uvElem && uvSetName)
      {
        int uvIdx = mesh->GetTextureUVIndex(p, v);
        FbxVector2 uv = (uvIdx >= 0)
          ? uvElem->GetDirectArray().GetAt(uvIdx)
          : readV2(uvElem, cpIndex, pvIndex);
        out.Tex = { (float)uv[0], 1.0f - (float)uv[1] };
      }
      else
      {
        out.Tex = { 0.0f, 0.0f };
      }

      cornerIdx.push_back((unsigned)vertices.size());
      vertices.push_back(out);
    }

    // Triangulación en “fan”
    for (int k = 1; k + 1 < polySize; ++k)
    {
      indices.push_back(cornerIdx[0]);
      indices.push_back(cornerIdx[k + 1]);
      indices.push_back(cornerIdx[k]);
    }
  }

  // Detección de espejo + corrección de winding
  bool autoDetectMirror = true;
  bool forceFlipWinding = true; // si queda mal, lo podemos poner en false

  bool mirrored = true;
  if (autoDetectMirror)
  {
    FbxAMatrix geo;
    geo.SetT(node->GetGeometricTranslation(FbxNode::eSourcePivot));
    geo.SetR(node->GetGeometricRotation(FbxNode::eSourcePivot));
    geo.SetS(node->GetGeometricScaling(FbxNode::eSourcePivot));
    FbxAMatrix world = node->EvaluateGlobalTransform() * geo;

    FbxVector4 S = world.GetS();
    double detScale = S[0] * S[1] * S[2];
    mirrored = (detScale < 0.0);
  }

  if (mirrored || forceFlipWinding)
  {
    for (size_t i = 0; i + 2 < indices.size(); i += 3)
      std::swap(indices[i + 1], indices[i + 2]);
  }

  // Empaquetamos en MeshComponent
  MeshComponent mc;
  mc.m_name = node->GetName();
  mc.m_vertex = std::move(vertices);
  mc.m_index = std::move(indices);
  mc.m_numVertex = (int)mc.m_vertex.size();
  mc.m_numIndex = (int)mc.m_index.size();

  if (mc.m_numVertex > 0 && mc.m_numIndex > 0)
    outMeshes.push_back(std::move(mc));
}

// -----------------------------------------------------------------------------
// Implementación de Model3D
// -----------------------------------------------------------------------------

Model3D::Model3D(const std::string& name, ModelType modelType)
  : IResource(name)
  , m_modelType(modelType)
{
  m_type = ResourceType::Model3D;
  m_state = ResourceState::Unloaded;
}

bool Model3D::load(const std::string& path)
{
  m_filePath = path;
  m_state = ResourceState::Loading;

  bool ok = false;
  m_meshes.clear();

  if (m_modelType == ModelType::OBJ)
  {
    ObjReader     reader;
    MeshComponent mesh;
    // ObjReader ya se encarga de añadir ".obj" si hace falta
    ok = reader.load(path, mesh, /*flipV=*/true);
    if (ok)
      m_meshes.push_back(std::move(mesh));
  }
  else if (m_modelType == ModelType::FBX)
  {
    ok = LoadFBXIntoMeshes(path, m_meshes);
  }

  m_state = ok ? ResourceState::Loaded : ResourceState::Failed;
  return ok;
}

bool Model3D::init()
{
  // Por ahora no hacemos nada en GPU aquí.
  return true;
}

void Model3D::unload()
{
  m_meshes.clear();
  m_state = ResourceState::Unloaded;
}

size_t Model3D::getSizeInBytes() const
{
  size_t total = 0;
  for (const auto& mc : m_meshes)
  {
    total += mc.m_vertex.size() * sizeof(SimpleVertex);
    total += mc.m_index.size() * sizeof(unsigned int);
  }
  return total;
}
