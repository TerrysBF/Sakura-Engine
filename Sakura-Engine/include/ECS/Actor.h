#pragma once
#include "Prerequisites.h"
#include "Entity.h"
#include "Buffer.h"
#include "Texture.h"
#include "Transform.h"
#include "SamplerState.h"
//#include "Rasterizer.h"
//#include "BlendState.h"
#include "ShaderProgram.h"
//#include "DepthStencilState.h"

class Device;
class DeviceContext;
class MeshComponent;

/// <summary>
/// Representa una entidad gráfica con mallas, texturas y recursos de renderizado.
/// Administra buffers de vértices/índices, texturas y estados básicos de dibujo.
/// </summary>
class
  Actor : public Entity {
public:
  /// <summary>
  /// Constructor por defecto.
  /// </summary>
  Actor() = default;

  /// <summary>
  /// Constructor que inicializa recursos básicos del actor usando un dispositivo.
  /// </summary>
  /// <param name="device">Dispositivo usado para crear recursos gráficos.</param>
  Actor(Device& device);

  /// <summary>
  /// Destructor virtual por defecto.
  /// </summary>
  virtual
    ~Actor() = default;

  /// <summary>
  /// Inicializa el actor.
  /// Método heredado de Entity. Se puede extender en clases derivadas.
  /// </summary>
  void
    init() override {}

  /// <summary>
  /// Actualiza el actor en cada frame.
  /// </summary>
  /// <param name="deltaTime">Tiempo transcurrido desde la última actualización.</param>
  /// <param name="deviceContext">Contexto del dispositivo para operaciones gráficas.</param>
  void
    update(float deltaTime, DeviceContext& deviceContext) override;

  /// <summary>
  /// Renderiza el actor en la escena.
  /// Configura estados y dibuja las mallas asociadas.
  /// </summary>
  /// <param name="deviceContext">Contexto del dispositivo para operaciones gráficas.</param>
  void
    render(DeviceContext& deviceContext) override;

  /// <summary>
  /// Libera los recursos asociados al actor (buffers, texturas, estados básicos).
  /// </summary>
  void
    destroy();

  /// <summary>
  /// Establece las mallas del actor y crea los buffers de vértices/índices.
  /// </summary>
  /// <param name="device">Dispositivo usado para inicializar los buffers.</param>
  /// <param name="meshes">Vector de componentes de malla.</param>
  void
    setMesh(Device& device, std::vector<MeshComponent> meshes);

  /// <summary>
  /// Obtiene el nombre del actor.
  /// </summary>
  /// <returns>Nombre actual del actor.</returns>
  std::string
    getName() { return m_name; }

  /// <summary>
  /// Establece el nombre del actor.
  /// </summary>
  /// <param name="name">Nuevo nombre.</param>
  void
    setName(const std::string& name) { m_name = name; }

  /// <summary>
  /// Establece la lista de texturas del actor.
  /// </summary>
  /// <param name="textures">Vector de texturas a asignar.</param>
  void
    setTextures(std::vector<Texture> textures) { m_textures = textures; }

  /// <summary>
  /// Define si el actor puede proyectar sombra.
  /// </summary>
  /// <param name="v">true para habilitar sombra, false para deshabilitar.</param>
  void
    setCastShadow(bool v) { castShadow = v; }

  /// <summary>
  /// Indica si el actor proyecta sombras.
  /// </summary>
  /// <returns>true si proyecta sombras, false en caso contrario.</returns>
  bool
    canCastShadow() const { return castShadow; }

  /// <summary>
  /// Renderiza la sombra del actor usando shaders y estados específicos.
  /// </summary>
  /// <param name="deviceContext">Contexto del dispositivo para operaciones gráficas.</param>
  void
    renderShadow(DeviceContext& deviceContext);

private:
  std::vector<MeshComponent> m_meshes;   // Conjunto de mallas del actor.
  std::vector<Texture> m_textures;       // Texturas aplicadas al actor.
  std::vector<Buffer> m_vertexBuffers;   // Buffers de vértices por malla.
  std::vector<Buffer> m_indexBuffers;    // Buffers de índices por malla.

  //BlendState m_blendstate;             // Estado de blending (no usado actualmente).
  //Rasterizer m_rasterizer;             // Estado de rasterización (no usado actualmente).
  SamplerState m_sampler;                // Sampler para las texturas del actor.
  CBChangesEveryFrame m_model;           // Datos por modelo (matriz mundo y color).
  Buffer m_modelBuffer;                  // Constant buffer que almacena m_model.

  // Recursos para sombras
  ShaderProgram m_shaderShadow;          // Shader program para renderizar sombras.
  Buffer m_shaderBuffer;                 // Buffer auxiliar para datos de sombra.
  //BlendState m_shadowBlendState;       // Estado de blending para sombras.
  //DepthStencilState m_shadowDepthStencilState; // Estado de profundidad/esténcil para sombras.
  CBChangesEveryFrame m_cbShadow;        // Datos específicos para el pase de sombras.

  XMFLOAT4 m_LightPos;                   // Posición de la luz para calcular la sombra.
  std::string m_name = "Actor";          // Nombre identificador del actor.
  bool castShadow = true;                // Indica si el actor proyecta sombra.
};
