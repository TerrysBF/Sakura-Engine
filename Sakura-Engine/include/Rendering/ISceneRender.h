/**
 * @file ISceneRenderer.h
 * @brief Declara una interfaz base para los renderizadores de escena.
 */
#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Camera;
class RenderScene;
class EditorViewportPass;
//class Texture;

/**
 * @enum RenderType
 * @brief Tipos de renderizado disponibles en el motor.
 */
enum class
  RenderType {
  Forward = 0,
  Deferred = 1
};

/**
 * @class ISceneRenderer
 * @brief Interfaz base para los renderizadores de escena.
 *
 * Esta clase define las funciones que debe implementar
 * cualquier renderer del motor, como Forward Renderer
 * o Deferred Renderer.
 */
class
  ISceneRenderer {
public:

  /**
   * @brief Destructor virtual.
   */
  virtual
    ~ISceneRenderer() = default;

  /**
   * @brief Inicializa los recursos necesarios del renderer.
   *
   * @param device Dispositivo gráfico utilizado para crear recursos.
   * @return HRESULT indicando si la operación fue exitosa.
   */
  virtual HRESULT
    init(Device& device) = 0;

  /**
   * @brief Actualiza el tamaño de los recursos de render.
   *
   * @param device Dispositivo gráfico.
   * @param width Nuevo ancho de render.
   * @param height Nueva altura de render.
   */
  virtual void
    resize(Device& device,
      unsigned int width,
      unsigned int height) = 0;

  /**
   * @brief Renderiza una escena completa.
   *
   * @param deviceContext Contexto del dispositivo.
   * @param camera Cámara activa.
   * @param scene Escena que será renderizada.
   * @param viewport Viewport donde se mostrará el resultado.
   */
  virtual void
    render(DeviceContext& deviceContext,
      const Camera& camera,
      RenderScene& scene,
      EditorViewportPass& viewport) = 0;

  /**
   * @brief Libera los recursos utilizados por el renderer.
   */
  virtual void
    destroy() = 0;

  /**
   * @brief Obtiene el recurso de sombras.
   *
   * @return Shader Resource View asociado a las sombras.
   */
  virtual ID3D11ShaderResourceView*
    getShadowMapSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de depuración previa a sombras.
   *
   * @return Shader Resource View generado por el renderer.
   */
  virtual ID3D11ShaderResourceView*
    getPreShadowSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de color y metalizado del G-Buffer.
   *
   * @return Shader Resource View correspondiente.
   */
  virtual ID3D11ShaderResourceView*
    getGBufferAlbedoMetallicSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de normales y rugosidad del G-Buffer.
   *
   * @return Shader Resource View correspondiente.
   */
  virtual ID3D11ShaderResourceView*
    getGBufferNormalRoughnessSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de posición y AO del G-Buffer.
   *
   * @return Shader Resource View correspondiente.
   */
  virtual ID3D11ShaderResourceView*
    getGBufferWorldAoSRV() const { return nullptr; }

  /**
   * @brief Obtiene la textura de emisión y transparencia del G-Buffer.
   *
   * @return Shader Resource View correspondiente.
   */
  virtual ID3D11ShaderResourceView*
    getGBufferEmissiveAlphaSRV() const { return nullptr; }

  /**
   * @brief Activa o desactiva la depuración de sombras.
   *
   * @param enabled Estado de la opción.
   */
  virtual void
    setShadowFactorDebugEnabled(bool enabled) { (void)enabled; }

  /**
   * @brief Cambia el modo de depuración del renderer.
   *
   * @param mode Modo seleccionado.
   */
  virtual void
    setDeferredDebugViewMode(int mode) { (void)mode; }

  /**
   * @brief Obtiene el nombre del renderer.
   *
   * @return Nombre del renderer actual.
   */
  virtual const char*
    getDebugName() const = 0;
};