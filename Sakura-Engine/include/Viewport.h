/**
 * @file Viewport.h
 * @brief Declara la API de Viewport dentro del subsistema Core.
 * @ingroup core
 */
#pragma once
#include "Prerequisites.h"

class Window;
class DeviceContext;

/**
 * @class Viewport
 * @brief Encapsula un @c D3D11_VIEWPORT para definir la regi?n de renderizado en la pantalla.
 *
 * Un viewport en Direct3D 11 especifica el ?rea rectangular del render target donde
 * se dibujar?n las primitivas. Incluye dimensiones, profundidad m?nima y m?xima,
 * as? como el origen en la superficie de render.
 *
 * Esta clase permite inicializar un viewport a partir de una ventana o de dimensiones
 * espec?ficas, y aplicarlo al pipeline gr?fico.
 */
class
  Viewport {
public:
  /**
   * @brief Constructor por defecto.
   */
  Viewport() = default;

  /**
   * @brief Destructor por defecto.
   */
  ~Viewport() = default;

  /**
   * @brief Inicializa el viewport a partir de una ventana.
   *
   * Utiliza el tama?o del cliente de la ventana para definir las dimensiones
   * del viewport.
   *
   * @param window Referencia a la ventana que define el ?rea de renderizado.
   * @return @c S_OK si la inicializaci?n fue exitosa.
   *
   * @post El miembro @c m_viewport contendr? las dimensiones de la ventana.
   */
  HRESULT
    init(const Window& window);

  /**
   * @brief Inicializa el viewport con dimensiones espec?ficas.
   *
   * Define un viewport con el ancho y alto especificados.
   * Los valores de profundidad m?nima y m?xima se establecen por defecto
   * en 0.0f y 1.0f respectivamente.
   *
   * @param width  Ancho del viewport en p?xeles.
   * @param height Alto del viewport en p?xeles.
   * @return @c S_OK si la inicializaci?n fue exitosa.
   */
  HRESULT
    init(unsigned int width, unsigned int height);

  /**
   * @brief Actualiza los par?metros del viewport.
   *
   * M?todo de marcador para futuras extensiones (por ejemplo,
   * manejo de redimensionado din?mico de la ventana).
   *
   * @note Actualmente no realiza ninguna operaci?n.
   */
  void
    update();

  /**
   * @brief Aplica el viewport al contexto de dispositivo.
   *
   * Llama a @c RSSetViewports para establecer este viewport
   * en la etapa de rasterizaci?n del pipeline.
   *
   * @param deviceContext Contexto de dispositivo donde se aplicar?.
   *
   * @pre El viewport debe haber sido inicializado con @c init().
   */
  void
    render(DeviceContext& deviceContext);

  /**
   * @brief Libera recursos asociados al viewport.
   *
   * En este caso, no hay recursos COM asociados, por lo que
   * la implementaci?n es vac?a.
   */
  void
    destroy() {}

public:
  /**
   * @brief Estructura de Direct3D que define el viewport.
   */
  D3D11_VIEWPORT m_viewport;
};

