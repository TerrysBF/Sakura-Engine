#pragma once
#include "Prerequisites.h"

// Forward declarations para no incluir headers completos
class Device;
class DeviceContext;
class Texture;

/*
 * Clase DepthStencilView
 *
 * Envuelve un ID3D11DepthStencilView de Direct3D.
 * Sirve para manejar la vista de profundidad (y esténcil) que se usa
 * junto con el render target al dibujar.
 * Aquí no se crea la textura de depth, solo la "view".
 */
class DepthStencilView {
public:
  // Constructor vacío, no crea ningún recurso todavía
  DepthStencilView() = default;

  // Destructor por defecto, no libera nada si no se llama a destroy()
  ~DepthStencilView() = default;

  /*
   * Crea la vista de profundidad/esténcil a partir de una textura.
   *
   * device       -> device de D3D ya creado
   * depthStencil -> textura que se va a usar como depth/stencil
   * format       -> formato de la DSV (por ejemplo DXGI_FORMAT_D24_UNORM_S8_UINT)
   *
   * Devuelve S_OK si se crea bien o un HRESULT de error si falla.
   */
  HRESULT init(Device& device, Texture& depthStencil, DXGI_FORMAT format);

  // Función de update vacía por ahora (por si se necesita en el futuro)
  void update() {};

  /*
   * Limpia la DSV (profundidad y esténcil) usando ClearDepthStencilView.
   *
   * deviceContext -> contexto donde se hace el clear.
   * La profundidad se pone en 1.0 y el valor de stencil en 0.
   */
  void render(DeviceContext& deviceContext);

  /*
   * Libera la vista de profundidad/esténcil.
   *
   * Se puede llamar varias veces sin causar problemas.
   * Después de esto m_depthStencilView queda en nullptr.
   */
  void destroy();

public:
  // Puntero a la vista de profundidad/esténcil de Direct3D
  ID3D11DepthStencilView* m_depthStencilView = nullptr;
};
