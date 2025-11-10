/**
 * @file SwapChain.cpp
 * @brief Implementación del swap chain: crear, conseguir back buffer, presentar y liberar.
 */

#include "SwapChain.h"
#include "Device.h"
#include "DeviceContext.h"
#include "Texture.h"
#include "Window.h"

 /**
  * Crea el swap chain y asocia el back buffer a una Texture.
  * Aquí también se crea el device y el device context y se configura MSAA.
  */
HRESULT
SwapChain::init(Device& device,
  DeviceContext& deviceContext,
  Texture& backBuffer,
  Window window) {
  // Primero se revisa que la ventana tenga un handle válido
  if (!window.m_hWnd) {
    ERROR("SwapChain", "init", "Invalid window handle. (m_hWnd is nullptr)");
    return E_POINTER;
  }

  HRESULT hr = S_OK;

  // Flags para crear el device. En debug se añade el flag de depuración.
  unsigned int createDeviceFlags = 0;
#ifdef _DEBUG
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // Lista de tipos de driver que se intentan en orden
  D3D_DRIVER_TYPE driverTypes[] = {
      D3D_DRIVER_TYPE_HARDWARE,   // GPU real
      D3D_DRIVER_TYPE_WARP,       // emulación rápida por CPU
      D3D_DRIVER_TYPE_REFERENCE,  // referencia (muy lenta)
  };
  unsigned int numDriverTypes = ARRAYSIZE(driverTypes);

  // Niveles de características de Direct3D que aceptamos
  D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
  };
  unsigned int numFeatureLevels = ARRAYSIZE(featureLevels);

  // Intentar crear el device y el contexto con los tipos de driver de la lista
  for (unsigned int driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
    D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];

    hr = D3D11CreateDevice(
      nullptr,                    // adaptador (null elige uno por defecto)
      driverType,                 // tipo de driver actual
      nullptr,                    // handle a DLL (no se usa)
      createDeviceFlags,          // flags de creación
      featureLevels,              // lista de feature levels
      numFeatureLevels,           // tamaño de la lista
      D3D11_SDK_VERSION,          // versión del SDK
      &device.m_device,           // sale el ID3D11Device
      &m_featureLevel,            // sale el feature level que se logró
      &deviceContext.m_deviceContext // sale el ID3D11DeviceContext
    );

    // Si se logró crear el device, se guarda el tipo de driver y se rompe el ciclo
    if (SUCCEEDED(hr)) {
      MESSAGE("SwapChain", "init", "Device created successfully.");
      m_driverType = driverType;
      break;
    }
  }

  // Si no se pudo crear el device con ningún driver, se regresa error
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to create D3D11 device. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Configurar MSAA con 4 muestras
  m_sampleCount = 4;
  hr = device.m_device->CheckMultisampleQualityLevels(
    DXGI_FORMAT_R8G8B8A8_UNORM, // formato típico de color
    m_sampleCount,              // muestras que queremos
    &m_qualityLevels            // aquí se guarda cuántos niveles soporta
  );

  // Si falla o no hay niveles de calidad, MSAA no es válido
  if (FAILED(hr) || m_qualityLevels == 0) {
    ERROR("SwapChain", "init",
      ("MSAA not supported or invalid quality level. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Descripción del swap chain, define cómo será el back buffer y cómo se presenta
  DXGI_SWAP_CHAIN_DESC sd;
  memset(&sd, 0, sizeof(sd));
  sd.BufferCount = 1;                                // un back buffer
  sd.BufferDesc.Width = window.m_width;              // ancho de la ventana
  sd.BufferDesc.Height = window.m_height;            // alto de la ventana
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // formato de color
  sd.BufferDesc.RefreshRate.Numerator = 60;          // 60 Hz
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // se usa como render target
  sd.OutputWindow = window.m_hWnd;                   // ventana donde se dibuja
  sd.Windowed = TRUE;                                // modo ventana
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;          // descarta contenido al hacer Present
  sd.SampleDesc.Count = m_sampleCount;               // cantidad de muestras MSAA
  sd.SampleDesc.Quality = m_qualityLevels - 1;       // nivel de calidad (0..quality-1)

  // A partir del device se obtienen las interfaces de DXGI
  // Primero se castea a IDXGIDevice
  hr = device.m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgiDevice);
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to query IDXGIDevice. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Con el IDXGIDevice se obtiene el adaptador (la GPU)
  hr = m_dxgiDevice->GetAdapter(&m_dxgiAdapter);
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to get IDXGIAdapter. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Con el adaptador se consigue la factory, que es la que crea el swap chain
  hr = m_dxgiAdapter->GetParent(
    __uuidof(IDXGIFactory),
    reinterpret_cast<void**>(&m_dxgiFactory));
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to get IDXGIFactory. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear el swap chain con la factory y la descripción
  hr = m_dxgiFactory->CreateSwapChain(device.m_device, &sd, &m_swapChain);
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to create swap chain. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Obtener el back buffer del swap chain como ID3D11Texture2D
  ID3D11Texture2D* pBackBuffer = nullptr;
  hr = m_swapChain->GetBuffer(
    0,                        // índice del buffer (0 = back buffer)
    __uuidof(ID3D11Texture2D),
    reinterpret_cast<void**>(&pBackBuffer));
  if (FAILED(hr)) {
    ERROR("SwapChain", "init",
      ("Failed to get back buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Guardar el back buffer dentro de la clase Texture
  // Aquí no se crea nada nuevo, solo se guarda el puntero
  backBuffer.m_texture = pBackBuffer;

  return S_OK;
}

// Libera el swap chain y las interfaces DXGI asociadas.
// Se llama al final del programa o cuando se quiere destruir todo.
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
 * Presenta el back buffer en la ventana.
 * Present(0, 0) significa sin sincronización con VSync.
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

// Métodos vacíos de update y render por si luego se quiere hacer algo
// relacionado con el swap chain cada frame.
void SwapChain::update() {}
void SwapChain::render() {}
