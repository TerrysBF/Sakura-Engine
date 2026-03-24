#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Window;
class Texture;

/**
 * @class SwapChain
 * @brief El sistema de "doble buffer" que evita que la imagen parpadee.
 * * Su trabajo es manejar dos imágenes: una que se está dibujando en secreto (Back Buffer)
 * y otra que el usuario está viendo (Front Buffer). Cuando la nueva está lista,
 * las intercambia (Swap) instantáneamente.
 */
class
  SwapChain {
public:
  /**
   * @brief Constructor: Prepara el objeto pero aún no crea la conexión con la GPU.
   */
  SwapChain() = default;

  /**
   * @brief Destructor: Se asegura de limpiar, aunque recuerda usar destroy().
   */
  ~SwapChain() = default;

  /**
   * @brief Crea el sistema de intercambio de imágenes para una ventana específica.
   * @param device El creador de recursos de la GPU.
   * @param deviceContext El ejecutor de órdenes.
   * @param backBuffer La textura donde vamos a pintar "detrás de cámaras".
   * @param window La ventana donde queremos que aparezca el dibujo.
   * @return S_OK si la tarjeta de video pudo crear el sistema de intercambio.
   */
  HRESULT
    init(Device& device,
      DeviceContext& deviceContext,
      Texture& backBuffer,
      Window window);

  /**
   * @brief Espacio para actualizaciones (actualmente no hace nada).
   */
  void
    update();

  /**
   * @brief Espacio para operaciones de renderizado especiales del SwapChain.
   */
  void
    render();

  /**
   * @brief Apaga el sistema y libera la memoria de la tarjeta de video.
   */
  void
    destroy();

  /**
   * @brief ¡El momento de la verdad! Intercambia los buffers para mostrar
   * lo que acabamos de pintar en la pantalla del usuario.
   */
  void
    present();

  /**
   * @brief Ajusta el tamaño de las imágenes internas cuando el usuario estira la ventana.
   */
  HRESULT
    resizeBuffers(UINT width, UINT height);

  /**
   * @brief Obtiene el acceso a la textura de dibujo (Back Buffer).
   */
  HRESULT
    getBackBuffer(Texture& backBuffer);

public:
  /** @brief El objeto real de DirectX 11 que controla el intercambio. */
  IDXGISwapChain* m_swapChain = nullptr;

  /** @brief Indica si estamos usando la tarjeta de video real o un simulador. */
  D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_NULL;

private:
  /** @brief La versión de DirectX que soporta tu tarjeta (ej: 11.0). */
  D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;

  /** @brief Cuántas muestras usamos para suavizar los bordes "serrucho" (Anti-Aliasing). */
  unsigned int m_sampleCount;

  /** @brief Calidad del suavizado de bordes. */
  unsigned int m_qualityLevels;

  // Estas son herramientas internas de Windows (DXGI) para hablar con la tarjeta de video:
  IDXGIDevice* m_dxgiDevice = nullptr;   ///< El dispositivo gráfico base.
  IDXGIAdapter* m_dxgiAdapter = nullptr; ///< Representa tu tarjeta de video física.
  IDXGIFactory* m_dxgiFactory = nullptr; ///< La "fábrica" que crea el SwapChain.
};