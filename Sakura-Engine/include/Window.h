#pragma once
#include "Prerequisites.h"

/**
 * @class Window
 * @brief Envoltura sencilla de una ventana Win32.
 *
 * Se encarga de crear la ventana, guardar su handle y tamaño,
 * y exponer métodos básicos de ciclo de vida.
 */
class
  Window {
public:
  Window() = default;
  ~Window() = default;

  /**
   * @brief Crea la ventana y la muestra.
   * @param hInstance Instancia de la app.
   * @param nCmdShow Modo de visualización (ShowWindow).
   * @param wndproc Procedimiento de ventana.
   * @return S_OK si ok; HRESULT en error.
   */
  HRESULT
    init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc);

  /**
   * @brief Actualiza estado de la ventana (placeholder).
   */
  void
    update();

  /**
   * @brief Dibujado relacionado con la ventana (placeholder).
   */
  void
    render();

  /**
   * @brief Destruye la ventana y limpia recursos.
   */
  void
    destroy();

public:
  /**
   * @brief Handle de ventana Win32.
   */
  HWND m_hWnd = nullptr;

  /**
   * @brief Dimensiones actuales de la ventana.
   */
  unsigned int m_width;
  unsigned int m_height;

private:
  /**
   * @brief Instancia de la aplicación.
   */
  HINSTANCE m_hInst = nullptr;

  /**
   * @brief Rect de la ventana (posición/tamaño).
   */
  RECT m_rect;

  /**
   * @brief Título de la ventana.
   */
  std::string m_windowName = "Wildvine Engine";
};
