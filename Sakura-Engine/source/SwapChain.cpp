/**
 * @file SwapChain.cpp
 * @brief Implementaci�n del swap chain: crear, conseguir back buffer, presentar y liberar.
 */

#include "SwapChain.h"
#include "Device.h"
#include "DeviceContext.h"
#include "Texture.h"
#include "Window.h"

 /**
  * @brief Crea el swap chain y obtiene el back buffer.
  * @param device Dispositivo D3D11.
  * @param deviceContext Contexto inmediato.
  * @param backBuffer Textura que representar� el back buffer.
  * @param window Ventana destino.
  * @return S_OK si sale bien HRESULT si sale error.
  * @note Usa 4x MSAA si est� disponible.
  */
HRESULT
SwapChain::init(Device& device,
  DeviceContext& deviceContext,
  Texture& backBuffer,
  Window window) {
  // Validar ventana
  if (!window.m_hWnd) {
    ERROR("SwapChain", "init", "Invalid window handle. (m_hWnd is nullptr)");
    return E_POINTER;
  }

  HRESULT hr = S_OK;

  // Flags del device
  unsigned int createDeviceFlags = 0;
#ifdef _DEBUG
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // Tipos de driver a intentar
  D3D_DRIVER_TYPE driverTypes[] = {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
  };
  unsigned int numDriverTypes = ARRAYSIZE(driverTypes);

  // Feature levels aceptados
  D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
  };
  unsigned int numFeatureLevels = ARRAYSIZE(featureLevels);

  // Crear el device/context
  for (unsigned int driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
    D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
    hr = D3D11CreateDevice(nullptr,
      driverType,
      nullptr,
      createDeviceFlags,
      featureLevels,
      numFeatureLevels,
      D3D11_SDK_VERSION,
      &device.m_device,
      &m_featureLevel,
      &deviceContext.m_deviceContext);

    if (SUCCEEDED(hr)) {
      MESSAGE("SwapChain", "init", "Device created successfully.");
      break;
    }
  }

  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to create D3D11 device. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Configurar MSAA (4x si est� disponible)
  m_sampleCount = 4;
  hr = device.m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM,
    m_sampleCount,
    &m_qualityLevels);
  if (FAILED(hr) || m_qualityLevels == 0) {
    ERROR("SwapChain", "init",
      ("MSAA not supported or invalid quality level. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Descripci�n del swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  memset(&sd, 0, sizeof(sd));
  sd.BufferCount = 1;
  sd.BufferDesc.Width = window.m_width;
  sd.BufferDesc.Height = window.m_height;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = window.m_hWnd;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  sd.SampleDesc.Count = m_sampleCount;
  sd.SampleDesc.Quality = m_qualityLevels - 1;

  // Obtener factory de DXGI (device -> adapter -> factory)
  hr = device.m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgiDevice);
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to query IDXGIDevice. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_dxgiDevice->GetAdapter(&m_dxgiAdapter);
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to get IDXGIAdapter. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_dxgiAdapter->GetParent(__uuidof(IDXGIFactory),
    reinterpret_cast<void**>(&m_dxgiFactory));
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to get IDXGIFactory. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear el swap chain
  hr = m_dxgiFactory->CreateSwapChain(device.m_device, &sd, &m_swapChain);

  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to create swap chain. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Obtener el back buffer (textura)
  hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
    reinterpret_cast<void**>(&backBuffer));
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to get back buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  return S_OK;
}

/**
 * @brief Libera swap chain y las interfaces DXGI usadas (device/adapter/factory).
 */
void
SwapChain::destroy() {
  if (m_swapChain) {
    SAFE_RELEASE(m_swapChain);
  }
  if (m_dxgiDevice) {
    SAFE_RELEASE(m_dxgiDevice);
  }
  if (m_dxgiAdapter) {
    SAFE_RELEASE(m_dxgiAdapter);
  }
  if (m_dxgiFactory) {
    SAFE_RELEASE(m_dxgiFactory);
  }
}

/**
 * @brief Presenta el back buffer en la ventana.
 * @note Present(0,0) es sin VSync; cambia los flags si quieres VSync.
 */
void
SwapChain::present() {
  if (m_swapChain) {
    HRESULT hr = m_swapChain->Present(0, 0);
    if (FAILED(hr)) {
      ERROR("SwapChain", "present",
        ("Failed to present swap chain. HRESULT: " + std::to_string(hr)).c_str());
    }
  }
  else {
    ERROR("SwapChain", "present", "Swap chain is not initialized.");
  }
}
