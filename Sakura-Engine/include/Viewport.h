#pragma once
#include "Prerequisites.h"

// Declaraciones adelantadas
class Window;
class DeviceContext;

/**
 * @class Viewport
 * @brief Define el área de dibujo dentro de la ventana.
 * * Imagina que la ventana es el marco de un cuadro y el Viewport es el lienzo.
 * Puedes hacer que el juego se dibuje en toda la ventana o solo en una esquina
 * (como cuando juegas en pantalla dividida).
 */
class Viewport {
public:
  /**
   * @brief Constructor: Crea un viewport vacío.
   */
  Viewport() = default;

  /**
   * @brief Destructor: No necesita liberar memoria especial.
   */
  ~Viewport() = default;

  /**
   * @brief Configura el área de dibujo para que ocupe toda la ventana actual.
   * @param window Referencia a la ventana para obtener su ancho y alto.
   * @return S_OK si se configuró correctamente.
   */
  HRESULT init(const Window& window);

  /**
   * @brief Configura el área de dibujo con un tamaño personalizado.
   * @param width Ancho en píxeles.
   * @param height Alto en píxeles.
   * @return S_OK si el tamaño es válido.
   */
  HRESULT init(unsigned int width, unsigned int height);

  /**
   * @brief Espacio para actualizaciones futuras (como cambios de tamaño).
   */
  void update();

  /**
   * @brief Le dice a la tarjeta de video qué área de la pantalla debe usar para pintar.
   * @param deviceContext El encargado de enviar esta configuración a la GPU.
   */
  void render(DeviceContext& deviceContext);

  /**
   * @brief No requiere liberar recursos de DirectX, pero se mantiene por consistencia.
   */
  void destroy() {}

public:
  /** * @brief Estructura interna de DirectX 11.
   * Guarda: Posición (X, Y), Tamaño (Width, Height) y Profundidad (Min/Max Depth).
   */
  D3D11_VIEWPORT m_viewport;
};