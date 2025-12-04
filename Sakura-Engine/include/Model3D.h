#pragma once
#include "Prerequisites.h"
#include "IResource.h"
#include "MeshComponent.h"

#include <string>
#include <vector>

// Tipo de modelo: OBJ o FBX.
enum ModelType {
  OBJ,
  FBX
};

/**
 * @brief Recurso de modelo 3D basado en MeshComponent.
 *
 * Envuelve uno o varios MeshComponent cargados desde disco.
 * No crea buffers de GPU; eso lo haces desde fuera (por ejemplo en BaseApp).
 */
class Model3D : public IResource
{
public:
  Model3D(const std::string& name, ModelType modelType);
  ~Model3D() override = default;

  // Carga el modelo desde disco
  bool load(const std::string& path) override;

  // Inicialización en GPU (por ahora no hace nada especial)
  bool init() override;

  // Libera los datos en CPU
  void unload() override;

  // Tamaño aproximado en bytes
  size_t getSizeInBytes() const override;

  // Acceso a las mallas cargadas
  const std::vector<MeshComponent>& GetMeshes() const { return m_meshes; }

  ModelType GetModelType() const { return m_modelType; }

private:
  ModelType m_modelType;
  std::vector<MeshComponent> m_meshes;
};
