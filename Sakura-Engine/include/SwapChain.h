#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Window;
class Texture;

// Clase que envuelve el IDXGISwapChain.
// El swap chain maneja los buffers que se dibujan y luego se muestran en la ventana.
class SwapChain {
public:
  // Constructor vacío (no hace nada todavía).
  SwapChain() = default;

  // Destructor por defecto. Los recursos se liberan con destroy().
  ~SwapChain() = default;

  // Crea el swap chain y obtiene el back buffer.
  // - device y deviceContext: se usan para crear D3D y el contexto.
  // - backBuffer: aquí guardamos la textura del back buffer.
  // - window: ventana donde se va a presentar.
  HRESULT init(Device& device,
    DeviceContext& deviceContext,
    Texture& backBuffer,
    Window window);

  // Update vacío por ahora (no lo usamos).
  void update();

  // Render vacío por ahora (solo está para futura lógica).
  void render();

  // Libera el swap chain y las interfaces DXGI relacionadas.
  void destroy();

  // Presenta el back buffer en la ventana (llama a Present).
  // Aquí se podría activar VSync cambiando los parámetros.
  void present();

  // Devuelve cuántas muestras usa el MSAA (para que coincida con depth buffer).
  unsigned int getSampleCount()   const { return m_sampleCount; }

  // Devuelve los niveles de calidad de MSAA soportados.
  unsigned int getQualityLevels() const { return m_qualityLevels; }

public:
  // Puntero al swap chain de D3D11.
  IDXGISwapChain* m_swapChain = nullptr;

  // Tipo de driver que se usó (HARDWARE, WARP, etc.).
  D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_NULL;

private:
  // Nivel de características de D3D que se consiguió (11.0, 10.1, etc.).
  D3D_FEATURE_LEVEL m_featureLevel = D3D_FEATURE_LEVEL_11_0;

  // Número de samples para MSAA (1 = sin MSAA).
  unsigned int m_sampleCount = 1;

  // Niveles de calidad disponibles para ese MSAA.
  unsigned int m_qualityLevels = 0;

  // Interfaz DXGI del device (para llegar al adapter/factory).
  IDXGIDevice* m_dxgiDevice = nullptr;

  // Interfaz DXGI del adaptador (GPU).
  IDXGIAdapter* m_dxgiAdapter = nullptr;

  // Interfaz DXGI de la factory (crea el swap chain).
  IDXGIFactory* m_dxgiFactory = nullptr;
};
