// Implementación de RenderTargetView:
// aquí se crea la RTV, se limpia, se hace bind al pipeline y se libera.

#include "RenderTargetView.h"
#include "Device.h"
#include "Texture.h"
#include "DeviceContext.h"
#include "DepthStencilView.h"

// Crea la Render Target View usando el back buffer del swap chain.
// Revisa que el device, la textura y el formato sean válidos,
// arma la descripción y llama a CreateRenderTargetView de D3D11.
HRESULT
RenderTargetView::init(Device& device, Texture& backBuffer, DXGI_FORMAT Format) {
  // Comprobación básica de punteros y formato
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

  // Descripción simple para una RTV de una textura 2D multisample
  D3D11_RENDER_TARGET_VIEW_DESC desc;
  memset(&desc, 0, sizeof(desc));
  desc.Format = Format;
  desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

  // Crear la RTV usando el device
  HRESULT hr = device.m_device->CreateRenderTargetView(
    backBuffer.m_texture,
    &desc,
    &m_renderTargetView);

  // Si falla, se reporta el error
  if (FAILED(hr)) {
    ERROR("RenderTargetView", "init",
      ("Failed to create render target view. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  return S_OK;
}

// Versión genérica de init: crea una RTV desde cualquier textura que se pase.
// Permite elegir la dimensión de la view (TEXTURE2D, TEXTURE2DMS, etc.).
HRESULT
RenderTargetView::init(Device& device,
  Texture& inTex,
  D3D11_RTV_DIMENSION ViewDimension,
  DXGI_FORMAT Format) {
  // Comprobación básica de argumentos
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

  // Descripción de la RTV según la dimensión que se pida
  D3D11_RENDER_TARGET_VIEW_DESC desc;
  memset(&desc, 0, sizeof(desc));
  desc.Format = Format;
  desc.ViewDimension = ViewDimension;

  // Crear la RTV
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

// Esta versión de render limpia el color del render target y
// además hace bind del RTV y del DSV al Output Merger.
// Se suele llamar al principio del frame.
void
RenderTargetView::render(DeviceContext& deviceContext,
  DepthStencilView& depthStencilView,
  unsigned int numViews,
  const float ClearColor[4]) {
  // Comprobar que el contexto y la RTV existan
  if (!deviceContext.m_deviceContext) {
    ERROR("RenderTargetView", "render", "DeviceContext is nullptr.");
    return;
  }
  if (!m_renderTargetView) {
    ERROR("RenderTargetView", "render", "RenderTargetView is nullptr.");
    return;
  }

  // Primero limpia el render target con el color que le pasamos
  deviceContext.m_deviceContext->ClearRenderTargetView(m_renderTargetView, ClearColor);

  // Luego hace bind del render target y del depth stencil al pipeline
  deviceContext.m_deviceContext->OMSetRenderTargets(
    numViews,
    &m_renderTargetView,
    depthStencilView.m_depthStencilView);
}

// Esta versión de render solo hace bind de la RTV al Output Merger,
// sin limpiar y sin usar ningún depth stencil.
// Útil cuando ya se limpió antes o para usar otra combinación de DSV.
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

// Libera el recurso COM de la RTV.
// SAFE_RELEASE aguanta que se llame varias veces sin crashear.
void
RenderTargetView::destroy() {
  SAFE_RELEASE(m_renderTargetView);
}
