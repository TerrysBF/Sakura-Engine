#pragma once
#include "Prerequisites.h"

// Declaraciones adelantadas: le avisamos al código que estas clases existen
class Device;
class DeviceContext;
class Texture;
class DepthStencilView;

/**
 * @class RenderTargetView
 * @brief Es el "Lienzo de Dibujo" principal del motor.
 * * Una textura es solo un conjunto de datos, pero la RenderTargetView le dice a DirectX:
 * "Usa esta textura específicamente para pintar los colores de mi escena".
 * Es como poner el papel sobre el caballete antes de empezar a usar los pinceles.
 */
class RenderTargetView {
public:
  /**
   * @brief Constructor: Prepara el objeto vacío.
   */
  RenderTargetView() = default;

  /**
   * @brief Destructor: Se asegura de que el objeto se limpie, aunque usa destroy() para la GPU.
   */
  ~RenderTargetView() = default;

  /**
   * @brief Conecta el Back Buffer del SwapChain para que podamos ver lo que dibujamos.
   * @param device El creador de recursos de la tarjeta de video.
   * @param backBuffer La textura que viene del SwapChain (donde se guarda el dibujo).
   * @param Format El formato del color (ej: RGBA de 8 bits).
   * @return S_OK si el lienzo se preparó correctamente.
   */
  HRESULT init(Device& device, Texture& backBuffer, DXGI_FORMAT Format);

  /**
   * @brief Crea un lienzo a partir de cualquier textura (útil para efectos especiales).
   * @param inTex La textura que queremos convertir en lienzo.
   * @param ViewDimension Indica si es una textura normal 2D o especial.
   * @param Format El formato de color.
   */
  HRESULT init(Device& device,
    Texture& inTex,
    D3D11_RTV_DIMENSION ViewDimension,
    DXGI_FORMAT Format);

  /**
   * @brief Espacio para actualizaciones futuras.
   */
  void update() {}

  /**
   * @brief "Limpia" el lienzo con un color sólido y lo prepara para dibujar.
   * * Es como pasar una esponja con pintura fresca antes de empezar el cuadro del nuevo frame.
   * @param deviceContext El encargado de dar las órdenes de dibujo.
   * @param depthStencilView El "juez de distancia" (para saber qué está delante de qué).
   * @param numViews Cuántos lienzos estamos activando (normalmente 1).
   * @param ClearColor El color de fondo (ej: {0.1f, 0.1f, 0.1f, 1.0f} para un gris oscuro).
   */
  void render(DeviceContext& deviceContext,
    DepthStencilView& depthStencilView,
    unsigned int numViews,
    const float ClearColor[4]);

  /**
   * @brief Activa el lienzo sin borrar lo que ya tenía pintado.
   */
  void render(DeviceContext& deviceContext,
    unsigned int numViews);

  /**
   * @brief Libera la memoria de la tarjeta de video.
   * * Es muy importante llamar a esto al cerrar el juego para no "ensuciar" la RAM de video.
   */
  void destroy();

private:
  /** @brief El recurso real de DirectX 11 que representa el destino del renderizado. */
  ID3D11RenderTargetView* m_renderTargetView = nullptr;
};