/**
 * @file DepthStencilView.cpp
 * @brief Implementación simple de DepthStencilView (crear, usar y liberar la DSV).
 */

#include "DepthStencilView.h"
#include "Device.h"
#include "DeviceContext.h"
#include "Texture.h"

 /**
	* @brief Crea la vista de profundidad/esténcil desde una textura.
	* @param device Dispositivo ya inicializado.
	* @param depthStencil Textura que será el depth/stencil.
	* @param format Formato de la view (ej. D24_S8, D32, etc.).
	* @return S_OK si salió bien; HRESULT en error.
	* @note Usa dimensión TEXTURE2DMS (pensado para MSAA).
	*/
HRESULT
DepthStencilView::init(Device& device, Texture& depthStencil, DXGI_FORMAT format) {
	if (!device.m_device) {
		ERROR("DepthStencilView", "init", "Device is null.");
	}
	if (!depthStencil.m_texture) {
		ERROR("DepthStencilView", "init", "Texture is null.");
		return E_FAIL;
	}

	// Desc básico para la DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset(&descDSV, 0, sizeof(descDSV));
	descDSV.Format = format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; // depth 2D multisample
	descDSV.Texture2D.MipSlice = 0;

	// Crear la DSV
	HRESULT hr = device.m_device->CreateDepthStencilView(
		depthStencil.m_texture,
		&descDSV,
		&m_depthStencilView);

	if (FAILED(hr)) {
		ERROR("DepthStencilView", "init",
			("Failed to create depth stencil view. HRESULT: " + std::to_string(hr)).c_str());
		return hr;
	}

	return S_OK;
}

/**
 * @brief Limpia la DSV (depth y stencil).
 * @param deviceContext Contexto donde se hará el clear.
 * @note Limpia a depth=1.0 y stencil=0.
 */
void
DepthStencilView::render(DeviceContext& deviceContext) {
	if (!deviceContext.m_deviceContext) {
		ERROR("DepthStencilView", "render", "Device context is null.");
		return;
	}

	// Clear de profundidad y esténcil
	deviceContext.m_deviceContext->ClearDepthStencilView(
		m_depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
}

/**
 * @brief Libera la vista de profundidad/esténcil.
 * @note Seguro de llamar varias veces.
 */
void
DepthStencilView::destroy() {
	SAFE_RELEASE(m_depthStencilView);
}
