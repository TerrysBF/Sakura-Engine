// Implementación básica para manejar la vista de profundidad (DepthStencilView):
// se crea desde una textura, se limpia cada frame y se libera al final.

#include "DepthStencilView.h"
#include "Device.h"
#include "DeviceContext.h"
#include "Texture.h"

// Función que crea la vista de profundidad/esténcil a partir de una textura.
// device: device de D3D11 ya inicializado.
// depthStencil: textura que se va a usar como depth/stencil.
// format: formato del depth (por ejemplo DXGI_FORMAT_D24_UNORM_S8_UINT).
HRESULT
DepthStencilView::init(Device& device, Texture& depthStencil, DXGI_FORMAT format) {
  // Comprobamos que el device exista.
  if (!device.m_device) {
    ERROR("DepthStencilView", "init", "Device is null.");
  }

  // Comprobamos que la textura de depth exista.
  if (!depthStencil.m_texture) {
    ERROR("DepthStencilView", "init", "Texture is null.");
    return E_FAIL;
  }

  // Descripción básica de la depth stencil view.
  // Aquí se indica el formato y que la textura es 2D multisample (TEXTURE2DMS).
  D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
  memset(&descDSV, 0, sizeof(descDSV));
  descDSV.Format = format;
  descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; // vista como textura 2D con MSAA
  descDSV.Texture2D.MipSlice = 0;

  // Crear la depth stencil view usando el device y la textura.
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

// Función que limpia el depth buffer y el stencil.
// Se suele llamar al inicio de cada frame.
// deviceContext: contexto que se usa para hacer el clear.
void
DepthStencilView::render(DeviceContext& deviceContext) {
  if (!deviceContext.m_deviceContext) {
    ERROR("DepthStencilView", "render", "Device context is null.");
    return;
  }

  // Limpia la vista de profundidad y esténcil.
  // D3D11_CLEAR_DEPTH limpia el z-buffer y D3D11_CLEAR_STENCIL limpia el stencil.
  // Depth = 1.0f significa "lo más lejos posible".
  deviceContext.m_deviceContext->ClearDepthStencilView(
    m_depthStencilView,
    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
    1.0f,
    0);
}

// Función que libera la depth stencil view.
// SAFE_RELEASE aguanta que el puntero sea nullptr, así que se puede llamar varias veces.
void
DepthStencilView::destroy() {
  SAFE_RELEASE(m_depthStencilView);
}
