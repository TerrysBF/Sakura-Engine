#pragma once
#include "Prerequisites.h"
#include "ECS/Entity.h"
#include "ECS/Transform.h"

#include "Buffer.h"
#include "Texture.h"
#include "SamplerState.h"
#include "ShaderProgram.h"

class Device;
class DeviceContext;
class MeshComponent;

/**
 * @class Actor
 * @brief Entidad gráfica con mallas, texturas y un Transform.
 */
class Actor : public Entity
{
public:
  Actor() = default;
  explicit Actor(Device& device);

  ~Actor() override = default;

  /// Por ahora no hacemos nada especial aquí, el trabajo está en los componentes.
  void init() override {}

  /// Actualiza todos los componentes y sube el World al constant buffer.
  void update(float deltaTime, DeviceContext& deviceContext) override;

  /// Renderiza todas las mallas del actor.
  void render(DeviceContext& deviceContext) override;

  /// Libera buffers y recursos.
  void destroy() override;

  /// Inicializa las mallas y crea un VB/IB por cada una.
  void setMesh(Device& device, std::vector<MeshComponent> meshes);

  std::string getName() const { return m_name; }
  void setName(const std::string& name) { m_name = name; }

  void setTextures(std::vector<Texture> textures) { m_textures = std::move(textures); }

  void setCastShadow(bool v) { castShadow = v; }
  bool canCastShadow() const { return castShadow; }

  /// Stub para sombras (de momento vacío, para no romper el linker).
  void renderShadow(DeviceContext& deviceContext);

private:
  std::vector<MeshComponent> m_meshes;
  std::vector<Texture>       m_textures;
  std::vector<Buffer>        m_vertexBuffers;
  std::vector<Buffer>        m_indexBuffers;

  SamplerState        m_sampler;
  CBChangesEveryFrame m_model;       ///< World + color
  Buffer              m_modelBuffer; ///< CB de m_model

  // Sombra (no lo usamos todavía, pero dejamos el campo para futuro)
  ShaderProgram       m_shaderShadow;
  Buffer              m_shaderBuffer;
  CBChangesEveryFrame m_cbShadow;

  XMFLOAT4  m_LightPos;
  std::string m_name = "Actor";
  bool        castShadow = true;
};
