#include "Model3D.h"
#include "OBJReader.h"
#include "MeshComponent.h"

#include <fbxsdk.h>
#include <string>

// Pequeña función helper para ver si una cadena termina con ".fbx" o ".FBX".
static bool EndsWithFBX(const std::string& s)
{
  if (s.size() < 4) return false;
  std::string ext = s.substr(s.size() - 4);
  return (ext == ".fbx" || ext == ".FBX");
}

// ---------------------------------------------------------------------
// Carga un archivo FBX usando FBX SDK y lo vuelca en uno o varios
// MeshComponent (uno por cada FbxMesh encontrado en la escena).
// ---------------------------------------------------------------------
static bool LoadFBXIntoMeshes(const std::string& basePath, std::vector<MeshComponent>& outMeshes)
{
  outMeshes.clear();

  // Asegurarnos de que el path termina en .fbx
  std::string filePath = basePath;
  if (!EndsWithFBX(filePath))
    filePath += ".fbx";

  // Crear el manager principal del FBX SDK
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

  // Crear la escena
  FbxScene* scene = FbxScene::Create(sdkManager, "scene");
  importer->Import(scene);
  importer->Destroy();

  // Triangular toda la geometría de la escena (para tener solo triángulos)
  FbxGeometryConverter converter(sdkManager);
  converter.Triangulate(scene, /*replace*/ true);

  FbxNode* root = scene->GetRootNode();
  if (!root)
  {
    scene->Destroy();
    sdkManager->Destroy();
    return false;
  }

  // Vamos recorriendo hijos directos. Si quieres ser más fancy,
  // puedes hacer un recorrido recursivo, pero para muchos casos
  // simples basta con esto.
  const int childCount = root->GetChildCount();
  for (int i = 0; i < childCount; ++i)
  {
    FbxNode* node = root->GetChild(i);
    if (!node) continue;

    FbxMesh* mesh = node->GetMesh();
    if (!mesh) continue;

    MeshComponent mc;
    mc.m_name = node->GetName();

    FbxVector4* controlPoints = mesh->GetControlPoints();
    const int polyCount = mesh->GetPolygonCount();

    // Preparar UVs
    FbxStringList uvSetNames;
    mesh->GetUVSetNames(uvSetNames);

    const char* uvSetName = (uvSetNames.GetCount() > 0) ? uvSetNames[0] : nullptr;
    const FbxGeometryElementUV* uvElement = nullptr;
    if (uvSetName)
      uvElement = mesh->GetElementUV(uvSetName);

    // Recorremos todos los polígonos (ya triangulados)
    for (int p = 0; p < polyCount; ++p)
    {
      const int polySize = mesh->GetPolygonSize(p); // debería ser 3
      for (int v = 0; v < polySize; ++v)
      {
        const int ctrlIndex = mesh->GetPolygonVertex(p, v);
        const FbxVector4 pos = controlPoints[ctrlIndex];

        SimpleVertex sv{};
        sv.Pos = XMFLOAT3(
          static_cast<float>(pos[0]),
          static_cast<float>(pos[1]),
          static_cast<float>(pos[2])
        );

        // UV por defecto
        sv.Tex = XMFLOAT2(0.0f, 0.0f);

        if (uvElement && uvSetName)
        {
          FbxVector2 uv;
          bool unmapped = false;
          if (mesh->GetPolygonVertexUV(p, v, uvSetName, uv, unmapped) && !unmapped)
          {
            sv.Tex.x = static_cast<float>(uv[0]);
            sv.Tex.y = 1.0f - static_cast<float>(uv[1]); // flip V para que encaje con D3D
          }
        }

        mc.m_vertex.push_back(sv);
        mc.m_index.push_back(static_cast<unsigned int>(mc.m_vertex.size() - 1));
      }
    }

    mc.m_numVertex = static_cast<int>(mc.m_vertex.size());
    mc.m_numIndex = static_cast<int>(mc.m_index.size());

    if (mc.m_numVertex > 0 && mc.m_numIndex > 0)
      outMeshes.push_back(std::move(mc));
  }

  const bool hasMeshes = !outMeshes.empty();

  scene->Destroy();
  sdkManager->Destroy();

  return hasMeshes;
}

// ---------------------------------------------------------------------
// Implementación de Model3D
// ---------------------------------------------------------------------

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
    ObjReader reader;
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
  // El que usa el modelo se encarga de crear buffers.
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
