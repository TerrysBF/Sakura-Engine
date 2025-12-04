#pragma once
#include "Prerequisites.h"

// Clase sencilla que envuelve una ventana Win32.
// Se encarga de crear la ventana, guardar el handle y el tamaño.
class Window
{
public:
  // Constructor por defecto: NO crea la ventana.
  Window() = default;

    // 🔒 Prohibir copias (evitamos DestroyWindow doble / prematuro)
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

  // Destructor: destruye la ventana y desregistra la clase.
  ~Window();

  // Crea y muestra la ventana.
  // Devuelve S_OK si todo salió bien, E_FAIL en caso contrario.
  HRESULT init(HINSTANCE hInstance,
    int nCmdShow,
    WNDPROC wndProc,
    unsigned int width = 1280,
    unsigned int height = 720,
    const std::wstring& title = L"Sakura-Engine");

  // Procesa los mensajes de Windows.
  // Devuelve false cuando se recibe WM_QUIT.
  bool processMessages();

  HWND     getHWND()   const { return m_hWnd; }
  unsigned getWidth()  const { return m_width; }
  unsigned getHeight() const { return m_height; }

  // Para no romper el código que ya usa m_width / m_height / m_hWnd
  // los dejamos públicos.
public:
  HWND        m_hWnd = nullptr;
  HINSTANCE   m_hInstance = nullptr;
  unsigned    m_width = 0;
  unsigned    m_height = 0;
  std::wstring m_title;
};
