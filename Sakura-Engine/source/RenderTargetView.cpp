/**
 * @file RenderTargetView.cpp
 * @brief Implementación sencilla del RTV: crear, limpiar/bind y liberar.
 */

#include "RenderTargetView.h"
#include "Device.h"
#include "Texture.h"
#include "DeviceContext.h"
#include "DepthStencilView.h"

 /**
	* @brief Crea la RTV desde el back buffer (multisample 2D).
	* @param device Device ya creado.
	* @param backBuffer Textura del swap chain.
	* @param Format Formato del RTV.
	* @return S_OK HRESULT en error.
	*/
HRESULT
RenderTargetView::init(Device& device, Texture& backBuffer, DXGI_FORMAT Format) {
	if (!device.m_device) {
		ERROR("RenderTargetView", "init", "Device is nullptr.");
		return E_POINTER;
	}
	if (!backBuffer.m_texture) {
		ERROR("RenderTargetView", "init", "Texture is nullptr.");
		return E_POINTER;
	}
	if (Format == DXGI_FORMAT_UNKNOWN) {
		ERROR("RenderTargetView", "init", "Format is DXGI_FORMAT_UNKNOWN.");
		return E_INVALIDARG;
	}

	// Desc básico para RTV (TEXTURE2DMS)
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = Format;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

	// Crear RTV
	HRESULT hr = device.m_device->CreateRenderTargetView(
		backBuffer.m_texture,
		&desc,
		&m_renderTargetView);
	if (FAILED(hr)) {
		ERROR("RenderTargetView", "init",
			("Failed to create render target view. HRESULT: " + std::to_string(hr)).c_str());
		return hr;
	}

	return S_OK;
}

/**
 * @brief Crea la RTV desde una textura cualquiera.
 * @param device Device ya creado.
 * @param inTex Textura base.
 * @param ViewDimension Dimensión (ej. TEXTURE2D / TEXTURE2DMS).
 * @param Format Formato de la view.
 * @return S_OK HRESULT en error.
 */
HRESULT
RenderTargetView::init(Device& device,
	Texture& inTex,
	D3D11_RTV_DIMENSION ViewDimension,
	DXGI_FORMAT Format) {
	if (!device.m_device) {
		ERROR("RenderTargetView", "init", "Device is nullptr.");
		return E_POINTER;
	}
	if (!inTex.m_texture) {
		ERROR("RenderTargetView", "init", "Texture is nullptr.");
		return E_POINTER;
	}
	if (Format == DXGI_FORMAT_UNKNOWN) {
		ERROR("RenderTargetView", "init", "Format is DXGI_FORMAT_UNKNOWN.");
		return E_INVALIDARG;
	}

	// Desc de RTV según la dimensión pedida
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Format = Format;
	desc.ViewDimension = ViewDimension;

	// Crear RTV
	HRESULT hr = device.m_device->CreateRenderTargetView(
		inTex.m_texture,
		&desc,
		&m_renderTargetView);

	if (FAILED(hr)) {
		ERROR("RenderTargetView", "init",
			("Failed to create render target view. HRESULT: " + std::to_string(hr)).c_str());
		return hr;
	}

	return S_OK;
}

/**
 * @brief Limpia la RTV con color y bindea RTV+DSV al OM.
 * @param deviceContext Contexto D3D11.
 * @param depthStencilView DSV a usar.
 * @param numViews Número de RTVs (normalmente 1).
 * @param ClearColor Color RGBA para el clear.
 */
void
RenderTargetView::render(DeviceContext& deviceContext,
	DepthStencilView& depthStencilView,
	unsigned int numViews,
	const float ClearColor[4]) {
	if (!deviceContext.m_deviceContext) {
		ERROR("RenderTargetView", "render", "DeviceContext is nullptr.");
		return;
	}
	if (!m_renderTargetView) {
		ERROR("RenderTargetView", "render", "RenderTargetView is nullptr.");
		return;
	}

	// Clear y bind de RTV + DSV
	deviceContext.m_deviceContext->ClearRenderTargetView(m_renderTargetView, ClearColor);
	deviceContext.m_deviceContext->OMSetRenderTargets(
		numViews,
		&m_renderTargetView,
		depthStencilView.m_depthStencilView);
}

/**
 * @brief Solo bindea la RTV (sin limpiar y sin DSV).
 * @param deviceContext Contexto D3D11.
 * @param numViews Número de RTVs (normalmente 1).
 */
void
RenderTargetView::render(DeviceContext& deviceContext, unsigned int numViews) {
	if (!deviceContext.m_deviceContext) {
		ERROR("RenderTargetView", "render", "DeviceContext is nullptr.");
		return;
	}
	if (!m_renderTargetView) {
		ERROR("RenderTargetView", "render", "RenderTargetView is nullptr.");
		return;
	}

	deviceContext.m_deviceContext->OMSetRenderTargets(
		numViews,
		&m_renderTargetView,
		nullptr);
}

/**
 * @brief Libera la RTV.
 * @note Se puede llamar varias veces sin problema.
 */
void RenderTargetView::destroy() {
	SAFE_RELEASE(m_renderTargetView);
}
