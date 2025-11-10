#pragma once
#include "Prerequisites.h"

// Declaraciones adelantadas
class Device;
class DeviceContext;
class Texture;
class DepthStencilView;

// Clase que envuelve un ID3D11RenderTargetView.
// Se encarga de crear, usar y liberar una render target view.
// No crea la textura, solo la vista que se usa para dibujar.
class RenderTargetView {
public:
  // Constructor por defecto, no hace nada especial.
  RenderTargetView() = default;

  // Destructor por defecto.
  // No libera el recurso por sí solo, para eso está destroy().
  ~RenderTargetView() = default;

  // Crea la render target view usando el back buffer del swap chain.
  // device: device de Direct3D.
  // backBuffer: textura que viene del swap chain.
  // Format: formato de la vista (por ejemplo R8G8B8A8_UNORM).
  // Devuelve S_OK si todo va bien o un HRESULT de error.
  HRESULT init(Device& device, Texture& backBuffer, DXGI_FORMAT Format);

  // Crea la render target view usando una textura cualquiera.
  // inTex: textura que se quiere usar como destino de render.
  // ViewDimension: tipo de vista (TEXTURE2D, TEXTURE2DMS, etc.).
  // Format: formato de la vista.
  HRESULT init(Device& device,
    Texture& inTex,
    D3D11_RTV_DIMENSION ViewDimension,
    DXGI_FORMAT Format);

  // Función de actualización (por ahora no hace nada).
  void update() {}

  // Limpia la RTV con un color y la bindea junto con la DSV.
  // También hace OMSetRenderTargets con esta RTV y el depth stencil.
  // numViews normalmente es 1.
  void render(DeviceContext& deviceContext,
    DepthStencilView& depthStencilView,
    unsigned int numViews,
    const float ClearColor[4]);

  // Bindea la RTV sin limpiar y sin usar depth stencil.
  // Solo llama OMSetRenderTargets con esta RTV y DSV nulo.
  void render(DeviceContext& deviceContext,
    unsigned int numViews);

  // Libera la render target view.
  // Usa SAFE_RELEASE, así que se puede llamar varias veces sin problema.
  void destroy();

private:
  // Puntero al ID3D11RenderTargetView real.
  // Se crea en init() y se libera en destroy().
  ID3D11RenderTargetView* m_renderTargetView = nullptr;
};
