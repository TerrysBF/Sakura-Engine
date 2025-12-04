#pragma once
#include "Prerequisites.h"

// Clase sencilla que envuelve una ventana Win32.
// Se encarga de crear la ventana, guardar el handle y el tamaño.
class Window {
public:
  // Constructor por defecto.
  Window() = default;

  // Destructor por defecto.
  ~Window() = default;

  // Crea y muestra la ventana.
  // hInstance: instancia de la app.
  // nCmdShow: cómo se muestra la ventana (ShowWindow).
  // wndproc: función de ventana.
  // Devuelve S_OK si todo va bien.
  HRESULT init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc);

  // Update de la ventana (por ahora no hace nada).
  void update();

  // Render ligado a la ventana (placeholder).
  void render();

  // Destruye la ventana y limpia recursos si hiciera falta.
  void destroy();

public:
  // Handle de la ventana Win32.
  HWND m_hWnd = nullptr;

  // Ancho y alto actuales del área cliente.
  unsigned int m_width;
  unsigned int m_height;

private:
  // Instancia de la aplicación.
  HINSTANCE m_hInst = nullptr;

  // Rect de la ventana (posición y tamaño en pantalla).
  RECT m_rect;

  // Título de la ventana.
  std::string m_windowName = "Sakura-Engine";
};
