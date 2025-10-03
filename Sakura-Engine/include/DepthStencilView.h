#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Texture;

/**
 * @class DepthStencilView
 * @brief Envoltura sencilla para usar un ID3D11DepthStencilView.
 *
 * Sirve para crear, usar y soltar la vista de profundidad que
 * va en la etapa final del render (OM). No crea la textura, solo la view.
 * Nota: no maneja la vida de Texture ni de DeviceContext.
 */
class DepthStencilView {
public:
  /**
   * @brief Constructor vac�o (no crea nada).
   */
  DepthStencilView() = default;

  /**
   * @brief Destructor por defecto.
   * @details No libera nada por s� solo; usa destroy() cuando ya no lo necesites.
   */
  ~DepthStencilView() = default;

  /**
   * @brief Crea la vista de profundidad/est�ncil desde una textura.
   *
   * @param device        Dispositivo con el que se crea la view.
   * @param depthStencil  Textura que ser� el depth/stencil (debe tener el bind de DEPTH_STENCIL).
   * @param format        Formato de la view (ej: DXGI_FORMAT_D24_UNORM_S8_UINT).
   * @return S_OK si sali� bien; HRESULT de error si fall�.
   *
   * @pre La textura ya existe y es compatible con el formato de la view.
   * @post Si todo ok, m_depthStencilView != nullptr.
   * @sa destroy()
   */
  HRESULT init(Device& device, Texture& depthStencil, DXGI_FORMAT format);

  /**
   * @brief Para futuras actualizaciones de estado (por ahora no hace nada).
   */
  void update() {};

  /**
   * @brief Enlaza la vista de profundidad/est�ncil al contexto para render.
   *
   * Llama a OMSetRenderTargets con esta DSV en el DeviceContext.
   * @param deviceContext Contexto donde se har� el bind.
   * @pre Debiste llamar init() antes.
   */
  void render(DeviceContext& deviceContext);

  /**
   * @brief Libera la view creada.
   *
   * Se puede llamar varias veces sin problema.
   * @post m_depthStencilView == nullptr.
   */
  void destroy();

public:
  /**
   * @brief Puntero a la vista de profundidad/est�ncil (D3D11).
   * @details Queda v�lido tras init(); vuelve a nullptr despu�s de destroy().
   */
  ID3D11DepthStencilView* m_depthStencilView = nullptr;
};
