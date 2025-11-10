// Implementación de funciones auxiliares para crear recursos usando ID3D11Device.

#include "Device.h"

// Libera el ID3D11Device almacenado en m_device.
// SAFE_RELEASE comprueba si es nullptr antes de llamar Release().
void
Device::destroy() {
  SAFE_RELEASE(m_device);
}

// Crea una Render Target View (RTV) a partir de un recurso (normalmente una textura).
// pResource: textura o recurso base.
// pDesc: descripción de la RTV (puede ser nullptr para usar valores por defecto).
// ppRTView: puntero de salida donde se guarda la RTV creada.
HRESULT
Device::CreateRenderTargetView(ID3D11Resource* pResource,
  const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
  ID3D11RenderTargetView** ppRTView) {
  // Validación básica de parámetros.
  if (!pResource) {
    ERROR("Device", "CreateRenderTargetView", "pResource is nullptr");
    return E_INVALIDARG;
  }
  if (!ppRTView) {
    ERROR("Device", "CreateRenderTargetView", "ppRTView is nullptr");
    return E_POINTER;
  }

  // Crear la RTV usando el device de D3D11.
  HRESULT hr = m_device->CreateRenderTargetView(pResource, pDesc, ppRTView);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateRenderTargetView",
      "Render Target View created successfully!");
  }
  else {
    ERROR("Device", "CreateRenderTargetView",
      ("Failed to create Render Target View. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

// Crea una textura 2D en GPU.
// pDesc: descripción de la textura (tamaño, formato, bind flags, etc.).
// pInitialData: datos iniciales, se puede pasar nullptr para crearla vacía.
// ppTexture2D: puntero de salida para la textura creada.
HRESULT
Device::CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc,
  const D3D11_SUBRESOURCE_DATA* pInitialData,
  ID3D11Texture2D** ppTexture2D) {
  // Validación básica.
  if (!pDesc) {
    ERROR("Device", "CreateTexture2D", "pDesc is nullptr");
    return E_INVALIDARG;
  }
  if (!ppTexture2D) {
    ERROR("Device", "CreateTexture2D", "ppTexture2D is nullptr");
    return E_POINTER;
  }

  // Crear la textura con el device.
  HRESULT hr = m_device->CreateTexture2D(pDesc, pInitialData, ppTexture2D);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateTexture2D",
      "Texture2D created successfully!");
  }
  else {
    ERROR("Device", "CreateTexture2D",
      ("Failed to create Texture2D. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

// Crea una Depth Stencil View (DSV) a partir de una textura de profundidad.
// pResource: recurso base, normalmente una textura 2D con bind de depth/stencil.
// pDesc: descripción de la DSV (puede ser nullptr).
// ppDepthStencilView: puntero de salida para la DSV creada.
HRESULT
Device::CreateDepthStencilView(ID3D11Resource* pResource,
  const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
  ID3D11DepthStencilView** ppDepthStencilView) {
  // Validación básica.
  if (!pResource) {
    ERROR("Device", "CreateDepthStencilView", "pResource is nullptr");
    return E_INVALIDARG;
  }
  if (!ppDepthStencilView) {
    ERROR("Device", "CreateDepthStencilView", "ppDepthStencilView is nullptr");
    return E_POINTER;
  }

  // Crear la DSV con el device.
  HRESULT hr = m_device->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateDepthStencilView",
      "Depth Stencil View created successfully!");
  }
  else {
    ERROR("Device", "CreateDepthStencilView",
      ("Failed to create Depth Stencil View. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

// Crea un Vertex Shader a partir de bytecode ya compilado.
// pShaderBytecode: puntero al buffer con el shader compilado.
// BytecodeLength: tamaño del buffer en bytes.
// pClassLinkage: normalmente nullptr.
// ppVertexShader: puntero de salida para el VS creado.
HRESULT
Device::CreateVertexShader(const void* pShaderBytecode,
  unsigned int BytecodeLength,
  ID3D11ClassLinkage* pClassLinkage,
  ID3D11VertexShader** ppVertexShader) {
  // Validación básica.
  if (!pShaderBytecode) {
    ERROR("Device", "CreateVertexShader", "pShaderBytecode is nullptr");
    return E_INVALIDARG;
  }
  if (!ppVertexShader) {
    ERROR("Device", "CreateVertexShader", "ppVertexShader is nullptr");
    return E_POINTER;
  }

  // Crear el vertex shader usando el device.
  HRESULT hr = m_device->CreateVertexShader(pShaderBytecode,
    BytecodeLength,
    pClassLinkage,
    ppVertexShader);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateVertexShader",
      "Vertex Shader created successfully!");
  }
  else {
    ERROR("Device", "CreateVertexShader",
      ("Failed to create Vertex Shader. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

// Crea un Input Layout que describe el formato de los vértices.
// pInputElementDescs: arreglo con las descripciones de cada elemento (POSITION, TEXCOORD, etc.).
// NumElements: número de elementos en el arreglo.
// pShaderBytecodeWithInputSignature: bytecode del VS que contiene la firma de entrada.
// BytecodeLength: tamaño del bytecode.
// ppInputLayout: puntero de salida para el input layout creado.
HRESULT
Device::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
  unsigned int NumElements,
  const void* pShaderBytecodeWithInputSignature,
  unsigned int BytecodeLength,
  ID3D11InputLayout** ppInputLayout) {
  // Validación básica.
  if (!pInputElementDescs) {
    ERROR("Device", "CreateInputLayout", "pInputElementDescs is nullptr");
    return E_INVALIDARG;
  }
  if (!ppInputLayout) {
    ERROR("Device", "CreateInputLayout", "ppInputLayout is nullptr");
    return E_POINTER;
  }

  // Crear el input layout con el device.
  HRESULT hr = m_device->CreateInputLayout(pInputElementDescs,
    NumElements,
    pShaderBytecodeWithInputSignature,
    BytecodeLength,
    ppInputLayout);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateInputLayout",
      "Input Layout created successfully!");
  }
  else {
    ERROR("Device", "CreateInputLayout",
      ("Failed to create Input Layout. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

// Crea un Pixel Shader a partir de bytecode ya compilado.
// pShaderBytecode: código compilado del pixel shader.
// BytecodeLength: tamaño del bytecode.
// pClassLinkage: normalmente nullptr.
// ppPixelShader: puntero de salida para el PS creado.
HRESULT
Device::CreatePixelShader(const void* pShaderBytecode,
  unsigned int BytecodeLength,
  ID3D11ClassLinkage* pClassLinkage,
  ID3D11PixelShader** ppPixelShader) {
  // Validación básica.
  if (!pShaderBytecode) {
    ERROR("Device", "CreatePixelShader", "pShaderBytecode is nullptr");
    return E_INVALIDARG;
  }
  if (!ppPixelShader) {
    ERROR("Device", "CreatePixelShader", "ppPixelShader is nullptr");
    return E_POINTER;
  }

  // Crear el pixel shader.
  HRESULT hr = m_device->CreatePixelShader(pShaderBytecode,
    BytecodeLength,
    pClassLinkage,
    ppPixelShader);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreatePixelShader",
      "Pixel Shader created successfully!");
  }
  else {
    ERROR("Device", "CreatePixelShader",
      ("Failed to create Pixel Shader. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

// Crea un Sampler State para el muestreo de texturas.
// pSamplerDesc: descripción del sampler (filtro, dirección, etc.).
// ppSamplerState: puntero de salida para el sampler creado.
HRESULT
Device::CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc,
  ID3D11SamplerState** ppSamplerState) {
  // Validación básica.
  if (!pSamplerDesc) {
    ERROR("Device", "CreateSamplerState", "pSamplerDesc is nullptr");
    return E_INVALIDARG;
  }
  if (!ppSamplerState) {
    ERROR("Device", "CreateSamplerState", "ppSamplerState is nullptr");
    return E_POINTER;
  }

  // Crear el sampler state.
  HRESULT hr = m_device->CreateSamplerState(pSamplerDesc, ppSamplerState);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateSamplerState",
      "Sampler State created successfully!");
  }
  else {
    ERROR("Device", "CreateSamplerState",
      ("Failed to create Sampler State. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}

// Crea un buffer genérico (vertex, index, constant, etc.).
// pDesc: descripción del buffer (ByteWidth, BindFlags, Usage, etc.).
// pInitialData: datos iniciales para llenar el buffer (puede ser nullptr).
// ppBuffer: puntero de salida donde se guarda el buffer creado.
HRESULT
Device::CreateBuffer(const D3D11_BUFFER_DESC* pDesc,
  const D3D11_SUBRESOURCE_DATA* pInitialData,
  ID3D11Buffer** ppBuffer) {
  // Validación básica.
  if (!pDesc) {
    ERROR("Device", "CreateBuffer", "pDesc is nullptr");
    return E_INVALIDARG;
  }
  if (!ppBuffer) {
    ERROR("Device", "CreateBuffer", "ppBuffer is nullptr");
    return E_POINTER;
  }

  // Crear el buffer con el device.
  HRESULT hr = m_device->CreateBuffer(pDesc, pInitialData, ppBuffer);

  if (SUCCEEDED(hr)) {
    MESSAGE("Device", "CreateBuffer",
      "Buffer created successfully!");
  }
  else {
    ERROR("Device", "CreateBuffer",
      ("Failed to create Buffer. HRESULT: " + std::to_string(hr)).c_str());
  }

  return hr;
}
