#pragma once
#include "Prerequisites.h"

// Forward Declarations
class Device;
class DeviceContext;
class Texture;
class DepthStencilView;

/**
 * @class RenderTargetView
 * @brief Wrapper sencillo para un ID3D11RenderTargetView.
 *
 * Sirve para crear, usar (bind/clear) y liberar un RTV.
 * No crea la textura, solo la view.
 */
class
  RenderTargetView {
public:
  /**
   * @brief Constructor por defecto.
   */
  RenderTargetView() = default;

  /**
   * @brief Destructor por defecto.
   * @details No libera el COM por sí solo; usa destroy().
   */
  ~RenderTargetView() = default;

  /**
   * @brief Crea la view desde el back buffer.
   * @param device Dispositivo D3D11.
   * @param backBuffer Textura del swap chain.
   * @param Format Formato (ej. DXGI_FORMAT_R8G8B8A8_UNORM).
   * @return S_OK si todo ok; HRESULT en error.
   * @post m_renderTargetView != nullptr en éxito.
   */
  HRESULT
    init(Device& device, Texture& backBuffer, DXGI_FORMAT Format);

  /**
   * @brief Crea la view desde una textura cualquiera.
   * @param device Dispositivo D3D11.
   * @param inTex Textura destino de render.
   * @param ViewDimension Dimensión de la view (ej. TEXTURE2D).
   * @param Format Formato de la view.
   * @return S_OK si todo ok; HRESULT en error.
   * @note Útil para targets auxiliares (G-Buffer, sombras, etc.).
   */
  HRESULT
    init(Device& device,
      Texture& inTex,
      D3D11_RTV_DIMENSION ViewDimension,
      DXGI_FORMAT Format);

  /**
   * @brief Placeholder para futuras actualizaciones (sin uso ahora).
   */
  void
    update();

  /**
   * @brief Hace bind del RTV y del DSV, y limpia el RTV con color.
   * @param deviceContext Contexto de D3D11.
   * @param depthStencilView DSV a asociar.
   * @param numViews Número de RTVs (normalmente 1).
   * @param ClearColor Color RGBA para el clear.
   * @pre Debes llamar init() antes.
   */
  void
    render(DeviceContext& deviceContext,
      DepthStencilView& depthStencilView,
      unsigned int numViews,
      const float ClearColor[4]);

  /**
   * @brief Hace bind del RTV sin limpiar y sin DSV.
   * @param deviceContext Contexto de D3D11.
   * @param numViews Número de RTVs (normalmente 1).
   * @pre Debes llamar init() antes.
   */
  void
    render(DeviceContext& deviceContext,
      unsigned int numViews);

  /**
   * @brief Libera la view creada (idempotente).
   * @post m_renderTargetView == nullptr.
   */
  void
    destroy();
private:
  /**
   * @brief Puntero COM a la RTV (válido tras init()).
   */
  ID3D11RenderTargetView* m_renderTargetView = nullptr;
};
