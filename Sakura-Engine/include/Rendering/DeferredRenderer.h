#pragma once
#include "Buffer.h"
#include "DepthStencilState.h"
#include "DepthStencilView.h"
#include "RasterizerState.h"
#include "Rendering/ISceneRender.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderTypes.h"
#include "SamplerState.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "EngineUtilities/Utilities/EditorViewportPass.h"

class Device;
class DeviceContext;
class Camera;
class Material;

/**
 * @class DeferredRenderer
 * @brief Renderer basado en Deferred Rendering.
 *
 * Esta clase se encarga de renderizar la escena utilizando
 * un G-Buffer para almacenar información de los objetos
 * antes de aplicar la iluminación.
 *
 * También administra sombras, objetos transparentes,
 * skybox y diferentes vistas de depuración.
 */
class
  DeferredRenderer : public ISceneRenderer {
public:

  /**
   * @brief Inicializa los recursos necesarios para el renderer.
   *
   * @param device Dispositivo gráfico utilizado para crear recursos.
   * @return HRESULT indicando si la operación fue exitosa.
   */
  HRESULT
    init(Device& device) override;

  /**
   * @brief Actualiza el tamaño de los buffers de render.
   *
   * @param device Dispositivo gráfico.
   * @param width Nuevo ancho de render.
   * @param height Nueva altura de render.
   */
  void
    resize(Device& device,
      unsigned int width,
      unsigned int height) override;

  /**
   * @brief Ejecuta el proceso completo de renderizado.
   *
   * @param deviceContext Contexto del dispositivo.
   * @param camera Cámara activa.
   * @param scene Escena a renderizar.
   * @param viewport Viewport donde se mostrará el resultado.
   */
  void
    render(DeviceContext& deviceContext,
      const Camera& camera,
      RenderScene& scene,
      EditorViewportPass& viewport) override;

  /**
   * @brief Libera todos los recursos utilizados por el renderer.
   */
  void
    destroy() override;

  /**
   * @brief Obtiene el Shader Resource View del mapa de sombras.
   *
   * @return Puntero al recurso de sombras.
   */
  ID3D11ShaderResourceView*
    getShadowMapSRV() const override { return m_shadowMapSRV.m_textureFromImg; }

  /**
   * @brief Obtiene el Shader Resource View del viewport de depuración.
   *
   * @return Puntero al recurso generado.
   */
  ID3D11ShaderResourceView*
    getPreShadowSRV() const override { return m_preShadowDebugPass.getSRVC(); }

  /**
   * @brief Obtiene el SRV del G-Buffer de color y metalizado.
   *
   * @return Puntero al recurso solicitado.
   */
  ID3D11ShaderResourceView*
    getGBufferAlbedoMetallicSRV() const override { return m_gBufferAlbedoMetallicSRV.m_textureFromImg; }

  /**
   * @brief Obtiene el SRV del G-Buffer de normales y rugosidad.
   *
   * @return Puntero al recurso solicitado.
   */
  ID3D11ShaderResourceView*
    getGBufferNormalRoughnessSRV() const override { return m_gBufferNormalRoughnessSRV.m_textureFromImg; }

  /**
   * @brief Obtiene el SRV del G-Buffer de posición y AO.
   *
   * @return Puntero al recurso solicitado.
   */
  ID3D11ShaderResourceView*
    getGBufferWorldAoSRV() const override { return m_gBufferWorldAoSRV.m_textureFromImg; }

  /**
   * @brief Obtiene el SRV del G-Buffer de emisión y alfa.
   *
   * @return Puntero al recurso solicitado.
   */
  ID3D11ShaderResourceView*
    getGBufferEmissiveAlphaSRV() const override { return m_gBufferEmissiveAlphaSRV.m_textureFromImg; }

  /**
   * @brief Activa o desactiva la depuración de sombras.
   *
   * @param enabled Estado de la opción.
   */
  void
    setShadowFactorDebugEnabled(bool enabled) override { m_shadowFactorDebugEnabled = enabled; }

  /**
   * @brief Cambia el modo de visualización de depuración.
   *
   * @param mode Modo seleccionado.
   */
  void
    setDeferredDebugViewMode(int mode) override { m_deferredDebugViewMode = mode; }

  /**
   * @brief Obtiene el nombre del renderer.
   *
   * @return Cadena con el nombre del renderer.
   */
  const char*
    getDebugName() const override { return "DeferredRenderer"; }

private:

  /**
   * @brief Construye las listas de objetos a renderizar.
   */
  void
    buildQueues(RenderScene& scene, const Camera& camera);

  /**
   * @brief Actualiza los datos que cambian cada frame.
   */
  void
    updatePerFrame(const Camera& camera,
      const RenderScene& scene,
      DeviceContext& deviceContext);

  /**
   * @brief Actualiza las matrices utilizadas para sombras.
   */
  void
    updateLightMatrices(const Camera& camera,
      const RenderScene& scene);

  /**
   * @brief Renderiza la escena en un render target específico.
   */
  void
    renderSceneToTarget(DeviceContext& deviceContext,
      RenderScene& scene,
      EditorViewportPass& targetPass,
      bool applyShadows);

  /**
   * @brief Configura los render targets del G-Buffer.
   */
  void
    bindGBufferTargets(DeviceContext& deviceContext,
      ID3D11DepthStencilView* depthStencilView);

  /**
   * @brief Configura el render target final.
   */
  void
    bindFinalTarget(DeviceContext& deviceContext,
      ID3D11RenderTargetView* renderTargetView,
      ID3D11DepthStencilView* depthStencilView);

  /**
   * @brief Limpia los recursos SRV utilizados por el renderer.
   */
  void
    clearDeferredSRVs(DeviceContext& deviceContext);

  /**
   * @brief Ejecuta el Geometry Pass.
   */
  void
    renderGeometryPass(DeviceContext& deviceContext);

  /**
   * @brief Renderiza un objeto durante el Geometry Pass.
   */
  void
    renderGeometryObject(DeviceContext& deviceContext,
      const RenderObject& object);

  /**
   * @brief Ejecuta el paso de iluminación.
   */
  void
    renderLightPass(DeviceContext& deviceContext);

  /**
   * @brief Renderiza el skybox de la escena.
   */
  void
    renderSkyboxPass(DeviceContext& deviceContext,
      RenderScene& scene);

  /**
   * @brief Renderiza los objetos transparentes.
   */
  void
    renderTransparentPass(DeviceContext& deviceContext);

  /**
   * @brief Renderiza un objeto utilizando Forward Rendering.
   */
  void
    renderForwardObject(DeviceContext& deviceContext,
      const RenderObject& object,
      RenderPassType passType);

  /**
   * @brief Ejecuta el paso de sombras.
   */
  void
    renderShadowPass(DeviceContext& deviceContext);

  /**
   * @brief Renderiza un objeto dentro del shadow pass.
   */
  void
    renderShadowObject(DeviceContext& deviceContext,
      const RenderObject& object);

  /**
   * @brief Crea los recursos necesarios para las sombras.
   */
  HRESULT
    createShadowResources(Device& device);

  /**
   * @brief Crea los recursos del G-Buffer.
   */
  HRESULT
    createGBufferResources(Device& device,
      unsigned int width,
      unsigned int height);

  /**
   * @brief Crea una textura y su render target asociado.
   */
  HRESULT
    createGBufferTargtes(Device& device,
      unsigned int width,
      unsigned int height,
      DXGI_FORMAT format,
      Texture& texture,
      Texture& srv,
      RenderTargetView& rtv);

  /**
   * @brief Crea los recursos utilizados para iluminación.
   */
  HRESULT
    createLightingResources(Device& device);

  /**
   * @brief Crea un quad de pantalla completa.
   */
  HRESULT
    createFullScreenQuad(Device& device);

  /**
   * @brief Crea los estados de mezcla utilizados por el renderer.
   */
  HRESULT
    createBlendStates(Device& device);

  /**
   * @brief Obtiene el estado de mezcla correspondiente a un material.
   */
  ID3D11BlendState*
    resolveBlendState(const Material& material) const;

private:

  /**
   * @brief Buffer constante utilizado por frame.
   */
  Buffer m_perFrameBuffer;

  /**
   * @brief Buffer constante utilizado por objeto.
   */
  Buffer m_perObjectBuffer;

  /**
   * @brief Buffer constante utilizado por material.
   */
  Buffer m_perMaterialBuffer;

  /**
   * @brief Buffer para datos de depuración.
   */
  Buffer m_lightingDebugBuffer;

  /**
   * @brief Buffer de vértices para pantalla completa.
   */
  Buffer m_fullscreenVertexBuffer;

  /**
   * @brief Buffer de índices para pantalla completa.
   */
  Buffer m_fullscreenIndexBuffer;

  /**
   * @brief Estado de profundidad para objetos transparentes.
   */
  DepthStencilState m_transparentDepthStencil;

  /**
   * @brief Estado de profundidad desactivado.
   */
  DepthStencilState m_disabledDepthStencil;

  /**
   * @brief Estado de profundidad utilizado para sombras.
   */
  DepthStencilState m_shadowDepthStencil;

  /**
   * @brief Estado de mezcla alfa.
   */
  ID3D11BlendState* m_alphaBlendState = nullptr;

  /**
   * @brief Estado de mezcla opaco.
   */
  ID3D11BlendState* m_opaqueBlendState = nullptr;

  /**
   * @brief Estado de mezcla aditivo.
   */
  ID3D11BlendState* m_additiveBlendState = nullptr;

  /**
   * @brief Estado de mezcla premultiplicado.
   */
  ID3D11BlendState* m_premultipliedBlendState = nullptr;

  /**
   * @brief Factor utilizado para operaciones de mezcla.
   */
  float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

  /**
   * @brief Tamaño del shadow map.
   */
  unsigned int m_shadowMapSize = 2048;

  /**
   * @brief Indica si las sombras están activadas.
   */
  bool m_aplyShadows = true;

  /**
   * @brief Ancho actual de render.
   */
  unsigned int m_renderWidth = 1280;

  /**
   * @brief Alto actual de render.
   */
  unsigned int m_renderHeight = 720;

  /**
   * @brief Activa la depuración del factor de sombra.
   */
  bool m_shadowFactorDebugEnabled = false;

  /**
   * @brief Modo de depuración seleccionado.
   */
  int m_deferredDebugViewMode = 0;

  /**
   * @brief Lista de objetos opacos.
   */
  std::vector<const RenderObject*> m_opaqueQueue;

  /**
   * @brief Lista de objetos transparentes.
   */
  std::vector<const RenderObject*> m_transparentQueue;
};