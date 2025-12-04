#include "Model3D.h"

/// <summary>
/// Carga el modelo desde la ruta indicada y actualiza el estado del recurso.
/// </summary>
/// <param name="path">Ruta del archivo del modelo.</param>
/// <returns>true si la carga fue exitosa; false en caso contrario.</returns>
bool
Model3D::load(const std::string& path) {
  SetPath(path);
  SetState(ResourceState::Loading);

  init();

  bool success = true; // Cambia esto según el resultado real.

  SetState(success ? ResourceState::Loaded : ResourceState::Failed);
  return success;
}

/// <summary>
/// Inicializa el modelo 3D y desencadena la carga del modelo FBX.
/// </summary>
/// <returns>true si la inicialización fue exitosa; false en caso contrario.</returns>
bool Model3D::init()
{
  // Inicializar recursos GPU, buffers, etc.
  LoadFBXModel(m_filePath);
  return false;
}

/// <summary>
/// Libera los datos asociados al modelo y marca el recurso como descargado.
/// </summary>
void Model3D::unload()
{
  // Liberar buffers, memoria en CPU/GPU, etc.
  SetState(ResourceState::Unloaded);
}

/// <summary>
/// Devuelve el tamaño estimado del modelo en memoria.
/// </summary>
/// <returns>Tamaño en bytes (por ahora 0, pendiente de implementar).</returns>
size_t Model3D::getSizeInBytes() const
{
  return 0;
}

/// <summary>
/// Inicializa el administrador y la escena del FBX SDK.
/// </summary>
/// <returns>true si la inicialización fue correcta; false en caso de error.</returns>
bool
Model3D::InitializeFBXManager() {
  // Initialize the FBX SDK manager
  lSdkManager = FbxManager::Create();
  if (!lSdkManager) {
    ERROR("ModelLoader", "FbxManager::Create()", "Unable to create FBX Manager!");
    return false;
  }
  else {
    MESSAGE("ModelLoader", "ModelLoader", "Autodesk FBX SDK version " << lSdkManager->GetVersion())
  }

  // Create an IOSettings object
  FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
  lSdkManager->SetIOSettings(ios);

  // Create an FBX Scene
  lScene = FbxScene::Create(lSdkManager, "MyScene");
  if (!lScene) {
    ERROR("ModelLoader", "FbxScene::Create()", "Unable to create FBX Scene!");
    return false;
  }
  else {
    MESSAGE("ModelLoader", "ModelLoader", "FBX Scene created successfully.")
  }
  return true;
}

/// <summary>
/// Carga un modelo FBX desde disco, procesa la escena y construye las mallas.
/// </summary>
/// <param name="filePath">Ruta del archivo FBX.</param>
/// <returns>Vector de MeshComponent generados a partir del modelo.</returns>
std::vector<MeshComponent>
Model3D::LoadFBXModel(const std::string& filePath) {
  // 01. Initialize the SDK from FBX Manager
  if (InitializeFBXManager()) {
    // 02. Create an importer using the SDK manager
    FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
    if (!lImporter) {
      ERROR("ModelLoader", "FbxImporter::Create()", "Unable to create FBX Importer!");
      return std::vector<MeshComponent>();
    }
    else {
      MESSAGE("ModelLoader", "ModelLoader", "FBX Importer created successfully.");
    }

    // 03. Use the first argument as the filename for the importer
    if (!lImporter->Initialize(filePath.c_str(), -1, lSdkManager->GetIOSettings())) {
      ERROR("ModelLoader", "FbxImporter::Initialize()",
        "Unable to initialize FBX Importer! Error: " << lImporter->GetStatus().GetErrorString());
      lImporter->Destroy();
      return std::vector<MeshComponent>();
    }
    else {
      MESSAGE("ModelLoader", "ModelLoader", "FBX Importer initialized successfully.");
    }

    // 04. Import the scene from the file into the scene
    if (!lImporter->Import(lScene)) {
      ERROR("ModelLoader", "FbxImporter::Import()",
        "Unable to import FBX Scene! Error: " << lImporter->GetStatus().GetErrorString());
      lImporter->Destroy();
      return std::vector<MeshComponent>();
    }
    else {
      MESSAGE("ModelLoader", "ModelLoader", "FBX Scene imported successfully.");
      m_name = lImporter->GetFileName();
    }

    FbxAxisSystem::DirectX.ConvertScene(lScene);
    FbxSystemUnit::m.ConvertScene(lScene);
    FbxGeometryConverter gc(lSdkManager);
    gc.Triangulate(lScene, /*replace*/ true);

    // 05. Destroy the importer
    lImporter->Destroy();
    MESSAGE("ModelLoader", "ModelLoader", "FBX Importer destroyed successfully.");

    // 06. Process the model from the scene
    FbxNode* lRootNode = lScene->GetRootNode();

    if (lRootNode) {
      MESSAGE("ModelLoader", "ModelLoader", "Processing model from the scene root node.");
      for (int i = 0; i < lRootNode->GetChildCount(); i++) {
        ProcessFBXNode(lRootNode->GetChild(i));
      }
      return m_meshes;
    }
    else {
      ERROR("ModelLoader", "FbxScene::GetRootNode()",
        "Unable to get root node from FBX Scene!");
      return std::vector<MeshComponent>();
    }
  }
  return m_meshes;
}

/// <summary>
/// Recorre un nodo del árbol FBX y procesa sus mallas,
/// además de llamar recursivamente a sus hijos.
/// </summary>
/// <param name="node">Nodo FBX a procesar.</param>
void
Model3D::ProcessFBXNode(FbxNode* node) {
  // 01. Process all the node's meshes
  if (node->GetNodeAttribute()) {
    if (node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh) {
      ProcessFBXMesh(node);
    }
  }

  // 02. Recursively process each child node
  for (int i = 0; i < node->GetChildCount(); i++) {
    ProcessFBXNode(node->GetChild(i));
  }
}

/// <summary>
/// Convierte una malla FBX en datos de vértices e índices,
/// construyendo un MeshComponent y agregándolo a la lista de mallas.
/// </summary>
/// <param name="node">Nodo FBX que contiene la malla.</param>
void
Model3D::ProcessFBXMesh(FbxNode* node) {
  FbxMesh* mesh = node->GetMesh();
  if (!mesh) return;

  // --- Asegura normales/tangentes en el FBX ---
  if (mesh->GetElementNormalCount() == 0)
    mesh->GenerateNormals(true, true);

  const char* uvSetName = nullptr;
  {
    FbxStringList uvSets; mesh->GetUVSetNames(uvSets);
    if (uvSets.GetCount() > 0) uvSetName = uvSets[0];
  }

  if (mesh->GetElementTangentCount() == 0 && uvSetName)
    mesh->GenerateTangentsData(uvSetName);

  const FbxGeometryElementUV* uvElem = (mesh->GetElementUVCount() > 0) ? mesh->GetElementUV(0) : nullptr;
  const FbxGeometryElementTangent* tanElem = (mesh->GetElementTangentCount() > 0) ? mesh->GetElementTangent(0) : nullptr;
  const FbxGeometryElementBinormal* binElem = (mesh->GetElementBinormalCount() > 0) ? mesh->GetElementBinormal(0) : nullptr;

  std::vector<SimpleVertex>       vertices;
  std::vector<unsigned int> indices;
  vertices.reserve(mesh->GetPolygonCount() * 3);
  indices.reserve(mesh->GetPolygonCount() * 3);

  // Helpers de lectura (control point vs. polygon-vertex)
  auto readV2 = [](const FbxGeometryElementUV* elem, int cpIdx, int pvIdx) -> FbxVector2 {
    if (!elem) return FbxVector2(0, 0);
    using E = FbxGeometryElement;
    int idx;
    if (elem->GetMappingMode() == E::eByControlPoint)
      idx = (elem->GetReferenceMode() == E::eIndexToDirect) ? elem->GetIndexArray().GetAt(cpIdx) : cpIdx;
    else
      idx = (elem->GetReferenceMode() == E::eIndexToDirect) ? elem->GetIndexArray().GetAt(pvIdx) : pvIdx;
    return elem->GetDirectArray().GetAt(idx);
    };
  auto readV4 = [](auto* elem, int cpIdx, int pvIdx) -> FbxVector4 {
    if (!elem) return FbxVector4(0, 0, 0, 0);
    using E = FbxGeometryElement;
    int idx;
    if (elem->GetMappingMode() == E::eByControlPoint)
      idx = (elem->GetReferenceMode() == E::eIndexToDirect) ? elem->GetIndexArray().GetAt(cpIdx) : cpIdx;
    else
      idx = (elem->GetReferenceMode() == E::eIndexToDirect) ? elem->GetIndexArray().GetAt(pvIdx) : pvIdx;
    return elem->GetDirectArray().GetAt(idx);
    };

  // --- Construcción por esquina (corner) ---
  for (int p = 0; p < mesh->GetPolygonCount(); ++p)
  {
    const int polySize = mesh->GetPolygonSize(p);
    std::vector<unsigned> cornerIdx; cornerIdx.reserve(polySize);

    for (int v = 0; v < polySize; ++v)
    {
      const int cpIndex = mesh->GetPolygonVertex(p, v);
      const int pvIndex = mesh->GetPolygonVertexIndex(p) + v;

      SimpleVertex out{};

      // Posición (espacio local)
      FbxVector4 P = mesh->GetControlPointAt(cpIndex);
      out.Pos = { (float)P[0], (float)P[1], (float)P[2] };

      // Normal por esquina (comentado actualmente)
      //FbxVector4 N(0, 1, 0, 0);
      //mesh->GetPolygonVertexNormal(p, v, N);
      //N.Normalize();
      //out.Normal = { (float)N[0], (float)N[1], (float)N[2] };

      // UV (invertir V para DX)
      if (uvElem && uvSetName) {
        int uvIdx = mesh->GetTextureUVIndex(p, v);
        FbxVector2 uv = (uvIdx >= 0) ? uvElem->GetDirectArray().GetAt(uvIdx)
          : readV2(uvElem, cpIndex, pvIndex);
        out.Tex = { (float)uv[0], 1.0f - (float)uv[1] };
      }
      else {
        out.Tex = { 0.0f, 0.0f };
      }

      // Tangente / Bitangente si existen (comentado actualmente)
      //if (tanElem) {
      //  FbxVector4 T = readV4(tanElem, cpIndex, pvIndex);
      //  out.Tangent = { (float)T[0], (float)T[1], (float)T[2] };
      //}
      //else out.Tangent = { 0,0,0 };
      //
      //if (binElem) {
      //  FbxVector4 B = readV4(binElem, cpIndex, pvIndex);
      //  out.Bitangent = { (float)B[0], (float)B[1], (float)B[2] };
      //}
      //else out.Bitangent = { 0,0,0 };

      cornerIdx.push_back((unsigned)vertices.size());
      vertices.push_back(out);
    }

    // Triangula en “fan” (CW por defecto)
    for (int k = 1; k + 1 < polySize; ++k) {
      indices.push_back(cornerIdx[0]);
      indices.push_back(cornerIdx[k + 1]);
      indices.push_back(cornerIdx[k]);
    }
  }

  // --- Fallback: cálculo de T/B comentado por ahora ---
  // ...

  // --- Autodetecta espejo global del nodo y corrige de forma CONSISTENTE ---
  bool autoDetectMirror = true;
  bool forceFlipWinding = true; // pon true si quieres forzar flip aunque no haya espejo

  bool mirrored = true;
  if (autoDetectMirror) {
    // world = global * geometric (aunque no lo horneamos a vértices, lo usamos para detectar espejo)
    FbxAMatrix geo;
    geo.SetT(node->GetGeometricTranslation(FbxNode::eSourcePivot));
    geo.SetR(node->GetGeometricRotation(FbxNode::eSourcePivot));
    geo.SetS(node->GetGeometricScaling(FbxNode::eSourcePivot));
    FbxAMatrix world = node->EvaluateGlobalTransform() * geo;

    // El signo del producto de escalas indica espejo
    FbxVector4 S = world.GetS();
    double detScale = S[0] * S[1] * S[2];
    mirrored = (detScale < 0.0);
  }

  if (mirrored || forceFlipWinding) {
    // 1) Flip global del winding (todas las caras)
    for (size_t i = 0; i + 2 < indices.size(); i += 3)
      std::swap(indices[i + 1], indices[i + 2]);

    // 2) Invertir normales/tangentes/bitangentes si se usan (comentado)
  }

  // --- Ortonormalización de TBN comentada por ahora ---
  // ...

  // --- Empaqueta en un MeshComponent y lo agrega al modelo ---
  MeshComponent mc;
  mc.m_name = node->GetName();
  mc.m_vertex = std::move(vertices);
  mc.m_index = std::move(indices);
  mc.m_numVertex = (int)mc.m_vertex.size();
  mc.m_numIndex = (int)mc.m_index.size();
  m_meshes.push_back(std::move(mc));
}

/// <summary>
/// Procesa el material de una malla FBX para obtener los nombres de texturas difusas.
/// </summary>
/// <param name="material">Material de superficie FBX a inspeccionar.</param>
void Model3D::ProcessFBXMaterials(FbxSurfaceMaterial* material)
{
  if (material) {
    FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
    if (prop.IsValid()) {
      int textureCount = prop.GetSrcObjectCount<FbxTexture>();
      for (int i = 0; i < textureCount; ++i) {
        FbxTexture* texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(i));
        if (texture) {
          textureFileNames.push_back(texture->GetName());
        }
      }
    }
  }
}
