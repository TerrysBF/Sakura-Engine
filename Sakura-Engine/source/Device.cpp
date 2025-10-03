/**
 * @file Device.cpp
 * @brief Implementación de helpers para crear recursos con ID3D11Device.
 */

#include "Device.h"

 /**
	* @brief Libera el device.
	*/
void
Device::destroy() {
	SAFE_RELEASE(m_device);
}

/**
 * @brief Crea una Render Target View (RTV).
 * @return S_OK si ok; HRESULT en error.
 */
HRESULT
Device::CreateRenderTargetView(ID3D11Resource* pResource,
	const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
	ID3D11RenderTargetView** ppRTView) {
	// Validación básica
	if (!pResource) {
		ERROR("Device", "CreateRenderTargetView", "pResource is nullptr");
		return E_INVALIDARG;
	}
	if (!ppRTView) {
		ERROR("Device", "CreateRenderTargetView", "ppRTView is nullptr");
		return E_POINTER;
	}

	// Crear la RTV
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

/**
 * @brief Crea una textura 2D.
 * @return S_OK si ok; HRESULT en error.
 */
HRESULT
Device::CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc,
	const D3D11_SUBRESOURCE_DATA* pInitialData,
	ID3D11Texture2D** ppTexture2D) {
	// Validación básica
	if (!pDesc) {
		ERROR("Device", "CreateTexture2D", "pDesc is nullptr");
		return E_INVALIDARG;
	}
	if (!ppTexture2D) {
		ERROR("Device", "CreateTexture2D", "ppTexture2D is nullptr");
		return E_POINTER;
	}

	// Crear la textura
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

/**
 * @brief Crea una Depth Stencil View (DSV).
 * @return S_OK si ok; HRESULT en error.
 */
HRESULT
Device::CreateDepthStencilView(ID3D11Resource* pResource,
	const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
	ID3D11DepthStencilView** ppDepthStencilView) {
	// Validación básica
	if (!pResource) {
		ERROR("Device", "CreateDepthStencilView", "pResource is nullptr");
		return E_INVALIDARG;
	}
	if (!ppDepthStencilView) {
		ERROR("Device", "CreateDepthStencilView", "ppDepthStencilView is nullptr");
		return E_POINTER;
	}

	// Crear la DSV
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

/**
 * @brief Crea un Vertex Shader.
 * @return S_OK si ok; HRESULT en error.
 */
HRESULT
Device::CreateVertexShader(const void* pShaderBytecode,
	unsigned int BytecodeLength,
	ID3D11ClassLinkage* pClassLinkage,
	ID3D11VertexShader** ppVertexShader) {
	// Validación básica
	if (!pShaderBytecode) {
		ERROR("Device", "CreateVertexShader", "pShaderBytecode is nullptr");
		return E_INVALIDARG;
	}
	if (!ppVertexShader) {
		ERROR("Device", "CreateVertexShader", "ppVertexShader is nullptr");
		return E_POINTER;
	}

	// Crear VS
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

/**
 * @brief Crea un Input Layout.
 * @return S_OK si ok; HRESULT en error.
 */
HRESULT
Device::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
	unsigned int NumElements,
	const void* pShaderBytecodeWithInputSignature,
	unsigned int BytecodeLength,
	ID3D11InputLayout** ppInputLayout) {
	// Validación básica
	if (!pInputElementDescs) {
		ERROR("Device", "CreateInputLayout", "pInputElementDescs is nullptr");
		return E_INVALIDARG;
	}
	if (!ppInputLayout) {
		ERROR("Device", "CreateInputLayout", "ppInputLayout is nullptr");
		return E_POINTER;
	}

	// Crear layout
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

/**
 * @brief Crea un Pixel Shader.
 * @return S_OK si ok; HRESULT en error.
 */
HRESULT
Device::CreatePixelShader(const void* pShaderBytecode,
	unsigned int BytecodeLength,
	ID3D11ClassLinkage* pClassLinkage,
	ID3D11PixelShader** ppPixelShader) {
	// Validación básica
	if (!pShaderBytecode) {
		ERROR("Device", "CreatePixelShader", "pShaderBytecode is nullptr");
		return E_INVALIDARG;
	}
	if (!ppPixelShader) {
		ERROR("Device", "CreatePixelShader", "ppPixelShader is nullptr");
		return E_POINTER;
	}

	// Crear PS
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

/**
 * @brief Crea un Sampler State.
 * @return S_OK si ok; HRESULT en error.
 */
HRESULT
Device::CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc,
	ID3D11SamplerState** ppSamplerState) {
	// Validación básica
	if (!pSamplerDesc) {
		ERROR("Device", "CreateSamplerState", "pSamplerDesc is nullptr");
		return E_INVALIDARG;
	}
	if (!ppSamplerState) {
		ERROR("Device", "CreateSamplerState", "ppSamplerState is nullptr");
		return E_POINTER;
	}

	// Crear sampler
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

/**
 * @brief Crea un buffer (vertex/index/constant, etc.).
 * @return S_OK si ok; HRESULT en error.
 */
HRESULT
Device::CreateBuffer(const D3D11_BUFFER_DESC* pDesc,
	const D3D11_SUBRESOURCE_DATA* pInitialData,
	ID3D11Buffer** ppBuffer) {
	// Validación básica
	if (!pDesc) {
		ERROR("Device", "CreateBuffer", "pDesc is nullptr");
		return E_INVALIDARG;
	}
	if (!ppBuffer) {
		ERROR("Device", "CreateBuffer", "ppBuffer is nullptr");
		return E_POINTER;
	}

	// Crear buffer
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
