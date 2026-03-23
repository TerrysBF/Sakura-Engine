#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;
class Texture;

/**
 * @class DepthStencilView
 * @brief Controla el "Buffer de Profundidad" (Z-Buffer).
 * * Es una regla que mide qué tan lejos está cada píxel de la cámara.
 * Gracias a esto, si una montaña está detrás de un árbol, el motor sabe que
 * no debe dibujar la montaña encima del árbol.
 */
class
  DepthStencilView {
public:
  /**
    * @brief Constructor: Prepara el objeto pero aún no crea nada en la tarjeta de video.
    */
  DepthStencilView() = default;

  /**
   * @brief Constructor: Prepara el objeto pero aún no crea nada en la tarjeta de video.
   */
  ~DepthStencilView() = default;

  /**
   * @brief Crea la "vista" de profundidad a partir de una textura.
   * @param device El creador de recursos de DirectX.
   * @param depthStencil La textura especial donde se guardarán las distancias (profundidad).
   * @param format El formato de los datos (qué tan precisos son los números de distancia).
   * @return S_OK si la tarjeta de video aceptó crear el buffer de profundidad.
   */
  HRESULT
    init(Device& device, Texture& depthStencil, DXGI_FORMAT format);

  HRESULT
    init(Device& device,
      Texture& depthStencil,
      DXGI_FORMAT format,
      D3D11_DSV_DIMENSION viewDimension);

  /**
   * @brief Por ahora no hace nada, pero sirve si quisiéramos cambiar algo en tiempo real.
   */
  void
    update() {};

  /**
   * @brief Prepara el sistema para usar este buffer de profundidad al dibujar.
   * @param deviceContext El encargado de dar las órdenes a la GPU.
   */
  void
    render(DeviceContext& deviceContext);

  /**
    * @brief Libera la memoria de la GPU. Es muy importante llamarlo al cerrar el programa
    * para no dejar "basura" en la tarjeta de video.
    */
  void
    destroy();

public:
  /** * @brief El recurso real de DirectX 11.
   * Es el puntero que conectamos directamente con la librería de Microsoft.
   */
  ID3D11DepthStencilView* m_depthStencilView = nullptr;
};