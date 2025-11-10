#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Window;
class Texture;

/**
 * @class SwapChain
 * @brief Envoltura simple de IDXGISwapChain.
 *
 * Administra los buffers que se muestran en pantalla (front/back).
 * Aquí lo creamos, lo usamos para presentar y luego lo liberamos.
 * Soporta MSAA (antialiasing por multisample).
 */
class
  SwapChain {
public:
  /**
   * @brief Constructor por defecto.
   */
  SwapChain() = default;

  /**
   * @brief Destructor por defecto.
   * @details No libera solo; usa destroy().
   */
  ~SwapChain() = default;

  /**
   * @brief Crea el swap chain y obtiene el back buffer.
   *
   * @param device Dispositivo D3D11.
   * @param deviceContext Contexto asociado.
   * @param backBuffer Textura que usaremos como back buffer.
   * @param window Ventana donde se presentará.
   * @return S_OK si ok; HRESULT en error.
   * @post m_swapChain != nullptr en éxito.
   */
  HRESULT
    init(Device& device,
      DeviceContext& deviceContext,
      Texture& backBuffer,
      Window window);

  /**
   * @brief Placeholder de actualización.
   */
  void
    update();

  /**
   * @brief Placeholder de render.
   */
  void
    render();

  /**
   * @brief Libera recursos del swap chain y DXGI.
   * @post m_swapChain == nullptr
   */
  void
    destroy();

  /**
   * @brief Presenta el back buffer en pantalla.
   * @note Aquí puedes controlar VSync (según implementación).
   */
  void
    present();

  /// Getters para MSAA (útiles al crear depth buffer)
  unsigned int getSampleCount()   const { return m_sampleCount; }
  unsigned int getQualityLevels() const { return m_qualityLevels; }

public:
  /**
   * @brief Swap chain de D3D11.
   */
  IDXGISwapChain* m_swapChain = nullptr;

  /**
   * @brief Tipo de driver (hardware, referencia, etc.).
   */
  D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_NULL;

private:
  /**
   * @brief Nivel de características de D3D alcanzado.
   */
  D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;

  /**
   * @brief Cantidad de muestras para MSAA (ej. 4 = 4x).
   */
  unsigned int m_sampleCount = 1;

  /**
   * @brief Niveles de calidad admitidos para MSAA.
   */
  unsigned int m_qualityLevels = 0;

  /**
   * @brief Interfaz DXGI del device.
   */
  IDXGIDevice* m_dxgiDevice = nullptr;

  /**
   * @brief Interfaz DXGI del adaptador (GPU).
   */
  IDXGIAdapter* m_dxgiAdapter = nullptr;

  /**
   * @brief Interfaz DXGI de la factory (crea swap chains).
   */
  IDXGIFactory* m_dxgiFactory = nullptr;
};
