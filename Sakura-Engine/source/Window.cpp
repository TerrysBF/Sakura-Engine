/**
 * @file Window.cpp
 * @brief Implementación básica de la ventana Win32 (crear/mostrar y tamaño).
 */

#include "Window.h"
#include "Device.h"
#include "BaseApp.h"

 /**
  * Inicializa la ventana de Windows y la muestra en pantalla.
  * Registra la clase de ventana, crea el HWND y guarda el tamaño del área de cliente.
  */
HRESULT
Window::init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc) {
  // Guardar el HINSTANCE que viene del sistema
  m_hInst = hInstance;

  // Datos de la clase de ventana (icono, cursor, callback, etc.)
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;     // se repinta si cambia ancho o alto
  wcex.lpfnWndProc = wndproc;               // función que manejará los mensajes (WndProc)
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = m_hInst;                 // instancia de la app
  wcex.hIcon = LoadIcon(m_hInst, (LPCTSTR)IDI_TUTORIAL1); // icono grande
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);             // cursor por defecto
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);        // color de fondo
  wcex.lpszMenuName = NULL;                // sin menú
  wcex.lpszClassName = "TutorialWindowClass"; // nombre de la clase
  wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1); // icono pequeño

  // Registrar la clase de ventana en el sistema
  if (!RegisterClassEx(&wcex))
    return E_FAIL;

  // Tamaño inicial deseado del área de cliente (ancho x alto)
  RECT rc = { 0, 0, 1200, 950 };
  m_rect = rc;

  // Ajustar el RECT para que el área de cliente quede del tamaño que queremos
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

  // Crear la ventana real
  m_hWnd = CreateWindow(
    "TutorialWindowClass",        // clase registrada arriba
    m_windowName.c_str(),         // título de la ventana (string de la clase)
    WS_OVERLAPPEDWINDOW,          // estilo típico de ventana
    CW_USEDEFAULT,                // posición X por defecto
    CW_USEDEFAULT,                // posición Y por defecto
    rc.right - rc.left,           // ancho total (con bordes)
    rc.bottom - rc.top,           // alto total (con bordes)
    NULL,                         // sin ventana padre
    NULL,                         // sin menú
    hInstance,                    // instancia
    NULL);                        // parámetros extra para WM_CREATE

  // Si algo salió mal, avisar y regresar error
  if (!m_hWnd) {
    MessageBox(nullptr, "CreateWindow failed!", "Error", MB_OK);
    ERROR("Window", "init", "CHECK FOR CreateWindow()");
    return E_FAIL;
  }

  // Mostrar la ventana en pantalla
  ShowWindow(m_hWnd, nCmdShow);
  UpdateWindow(m_hWnd);

  // Obtener el tamaño del área de cliente (donde se dibuja DirectX)
  GetClientRect(m_hWnd, &m_rect);
  m_width = m_rect.right - m_rect.left;   // ancho del cliente
  m_height = m_rect.bottom - m_rect.top;  // alto del cliente

  return S_OK;
}

/**
 * Función de actualización de la ventana.
 * Por ahora no hace nada, pero se podría usar para manejar lógica propia.
 */
void
Window::update() {}

/**
 * Función de render relacionada a la ventana.
 * Aquí no se dibuja nada directamente; DirectX se usa en otra parte.
 */
void
Window::render() {}

/**
 * Limpieza de la ventana.
 * Aquí se podría destruir recursos propios si se agregan después.
 */
void
Window::destroy() {}
