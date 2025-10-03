/**
 * @file Window.cpp
 * @brief Implementación básica de la ventana Win32 (crear/mostrar y tamaño).
 */

#include "Window.h"
#include "Device.h"

 /**
  * @brief Crea y muestra la ventana.
  * @return S_OK si salió bien; error en fallo.
  */
HRESULT
Window::init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc) {
  // Guardar instancia
  m_hInst = hInstance;

  // Registrar clase de ventana
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = wndproc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = m_hInst;
  wcex.hIcon = LoadIcon(m_hInst, (LPCTSTR)IDI_TUTORIAL1);
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = "TutorialWindowClass";
  wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
  if (!RegisterClassEx(&wcex))
    return E_FAIL;

  // Crear ventana
  RECT rc = { 0, 0, 1200, 950 };
  m_rect = rc;

  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

  m_hWnd = CreateWindow("TutorialWindowClass",
    m_windowName.c_str(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    rc.right - rc.left,
    rc.bottom - rc.top,
    NULL,
    NULL,
    hInstance,
    NULL);

  if (!m_hWnd) {
    MessageBox(nullptr, "CreateWindow failed!", "Error", MB_OK);
    ERROR("Window", "init", "CHECK FOR CreateWindow()");
    return E_FAIL;
  }

  ShowWindow(m_hWnd, nCmdShow);
  UpdateWindow(m_hWnd);

  // Guardar tamaño cliente
  GetClientRect(m_hWnd, &m_rect);
  m_width = m_rect.right - m_rect.left;
  m_height = m_rect.bottom - m_rect.top;

  return S_OK;
}

/**
 * @brief Update de la ventana (placeholder).
 */
void
Window::update() {}

/**
 * @brief Render relacionado a la ventana (placeholder).
 */
void
Window::render() {}

/**
 * @brief Limpieza de la ventana (placeholder).
 */
void
Window::destroy() {}
