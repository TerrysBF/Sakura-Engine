// Model3D.cpp - Implementación simple basada en OBJ, sin FBX SDK

#include "Model3D.h"
#include "ObjReader.h"

Model3D::Model3D(const std::string& name, ModelType modelType)
  : IResource(name)
  , m_modelType(modelType)
{
  // Configuramos los metadatos básicos del recurso
  m_type = ResourceType::Model3D;
  m_state = ResourceState::Unloaded;
  m_id = 0; // si luego quieres, puedes usar GenerateID() desde IResource
}

bool Model3D::load(const std::string& path)
{
  ObjReader reader;
  MeshComponent meshCPU;

  // Usamos el mismo loader que ya te funciona en BaseApp
  // Nota: si tu ObjReader espera "Alien" y le agrega ".obj" adentro,
  // aquí pásale la cadena igual que en BaseApp.
  if (!reader.load(path, meshCPU, true))
  {
    m_state = ResourceState::Failed;
    return false;
  }

  m_meshes.clear();
  m_meshes.push_back(std::move(meshCPU));

  m_filePath = path;
  m_state = ResourceState::Loaded;
  return true;
}

bool Model3D::init()
{
  // Aquí normalmente crearías buffers de GPU, etc.
  // De momento solo devolvemos true si la malla está cargada en CPU.
  return (m_state == ResourceState::Loaded);
}

void Model3D::unload()
{
  m_meshes.clear();
  m_filePath.clear();
  m_state = ResourceState::Unloaded;
}

size_t Model3D::getSizeInBytes() const
{
  // Si quieres algo más preciso, puedes sumar los tamaños de los arrays
  // de vértices/índices dentro de MeshComponent. Por ahora lo dejamos en 0.
  return 0;
}
