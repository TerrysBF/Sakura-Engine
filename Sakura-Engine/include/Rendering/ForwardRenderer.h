#pragma once
#include "Prerequisites.h"
#include "Buffer.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderTypes.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

class Device;
class DeviceContext;
class Camera;
class Material;

/**
 * @class ForwardRenderer
 * @brief Renderer tipo forward que se encarga de dibujar la escena por etapas.
 *
 * Maneja distintos passes como sombras, objetos opacos,
 * transparentes y skybox, además de preparar buffers y estados.
 */
class
  ForwardRenderer {
public:
  /**
   * @brief Inicializa el renderer.
   *
   * Crea buffers, estados y recursos necesarios para renderizar.
   */
  HRESULT init(Device& device);

  /**
   * @brief Actualiza datos que cambian cada frame.
   *
   * Se encarga de actualizar los constant buffers con la información
   * de la cámara y la escena.
   */
  void
    updatePerFrame(const Camera& camera,
      const RenderScene& scene,
      DeviceContext& deviceContext);

  /**
   * @brief Ejecuta todo el proceso de render.
   *
   * Llama a los distintos passes como sombras, opacos,
   * transparentes y skybox.
   */
  void
    render(DeviceContext& deviceContext,
      const Camera& camera,
      RenderScene& scene,
      EditorViewportPass& viewportPass);

  /**
   * @brief Obtiene la textura de sombras.
   */
  ID3D11ShaderResourceView* getShadowMapSRV() const { return m_shadowDepthSRV.m_textureFromImg; }

  /**
   * @brief Obtiene la textura del debug previo a sombras.
   */
  ID3D11ShaderResourceView* getPreShadowSRV() const { return m_preShadowDebugPass.getSRV(); }

private:
  /**
   * @brief Separa los objetos en opacos y transparentes.
   */
  void
    buildQueues(RenderScene& scene, const Camera& camera);

  /**
   * @brief Render de debug antes del pass de sombras.
   */
  void
    renderPreShadowDebugPass(DeviceContext& deviceContext, RenderScene& scene);

  /// Renderiza el pass de sombras
  void
    renderShadowPass(DeviceContext& deviceContext);

  /// Renderiza objetos opacos
  void
    renderOpaquePass(DeviceContext& deviceContext);

  /// Renderiza objetos transparentes
  void
    renderTransparentPass(DeviceContext& deviceContext);

  /**
   * @brief Renderiza el skybox de la escena.
   */
  void
    renderSkyboxPass(DeviceContext& deviceContext, RenderScene& scene);

  /**
   * @brief Renderiza un objeto dependiendo del pass.
   */
  void
    renderObject(DeviceContext& deviceContext, const RenderObject& object, RenderPassType passType);

  /**
   * @brief Renderiza un objeto en el pass de sombras.
   */
  void
    renderShadowObject(DeviceContext& deviceContext, const RenderObject& object);

  /**
   * @brief Crea los recursos necesarios para sombras.
   */
  HRESULT createShadowResources(Device& device);

  /**
   * @brief Actualiza matrices usadas para iluminación y sombras.
   */
  void
    updateLightMatrices(const Camera& camera, const RenderScene& scene);

  /**
   * @brief Crea estados de blending.
   */
  HRESULT createBlendStates(Device& device);

  /**
   * @brief Selecciona el estado de blending según el material.
   */
  ID3D11BlendState* resolveBlendState(const Material* material) const;

private:
  // Buffers principales
  Buffer m_perFrameBuffer;     // Datos generales por frame
  Buffer m_perObjectBuffer;    // Datos por objeto
  Buffer m_perMaterialBuffer;  // Datos por material

  // Estados de profundidad
  DepthStencilState m_transparentDepthStencil; // Para transparencias
  DepthStencilState m_shadowDepthStencil;      // Para sombras

  // Estados de blending
  ID3D11BlendState* m_alphaBlendState = nullptr;
  ID3D11BlendState* m_opaqueBlendState = nullptr;
  ID3D11BlendState* m_additiveBlendState = nullptr;
  ID3D11BlendState* m_premultipliedBlendState = nullptr;

  // Factor de mezcla
  float
    m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

  // Recursos de sombras
  Texture m_shadowDepthTexture;   // Textura de profundidad
  Texture m_shadowDepthSRV;       // Vista para shader
  DepthStencilView m_shadowDSV;   // Vista de depth
  ShaderProgram m_shadowShader;   // Shader de sombras
  RasterizerState m_shadowRasterizer; // Rasterizer para sombras

  unsigned
    int m_shadowMapSize = 2048; // Tamaño del shadow map

  // Debug
  EditorViewportPass m_preShadowDebugPass;

 
  bool
    m_applyShadows = true; 

  // Buffers CPU
  CBPerFrame m_cbPerFrame{};
  CBPerObject m_cbPerObject{};
  CBPerMaterial m_cbPerMaterial{};

  // Listas de render
  std::vector<const RenderObject*> m_opaqueQueue;      
  std::vector<const RenderObject*> m_transparentQueue; 
};