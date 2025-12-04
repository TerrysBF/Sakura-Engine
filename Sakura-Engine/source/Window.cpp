#include "Window.h"
#include "Prerequisites.h"

HRESULT Window::init(HINSTANCE hInstance,
  int nCmdShow,
  WNDPROC wndProc,
  unsigned int width,
  unsigned int height,
  const std::wstring& title)
{
  m_hInstance = hInstance;
  m_width = width;
  m_height = height;
  m_title = title;

  // Registrar clase de ventana
  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(WNDCLASSEXW);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = wndProc;                // <-- Usamos el WndProc que pasa BaseApp
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_hInstance;
  wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = L"SakuraWindowClass";
  wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

  if (!RegisterClassExW(&wc))
    return E_FAIL;

  // Ajustar rectángulo para que el área cliente sea width x height
  RECT rc = { 0, 0, (LONG)m_width, (LONG)m_height };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

  const int winWidth = rc.right - rc.left;
  const int winHeight = rc.bottom - rc.top;

  // Crear la ventana
  m_hWnd = CreateWindowExW(
    0,
    L"SakuraWindowClass",
    m_title.c_str(),
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    winWidth, winHeight,
    nullptr,
    nullptr,
    m_hInstance,
    nullptr
  );

  if (!m_hWnd)
  {
    MessageBoxW(nullptr,
      L"Error al crear la ventana principal.",
      L"Sakura-Engine",
      MB_ICONERROR | MB_OK);
    return E_FAIL;
  }

  ShowWindow(m_hWnd, nCmdShow);
  UpdateWindow(m_hWnd);

  return S_OK;
}

Window::~Window()
{
  if (m_hWnd)
  {
    DestroyWindow(m_hWnd);
    m_hWnd = nullptr;
  }

  if (m_hInstance)
  {
    UnregisterClassW(L"SakuraWindowClass", m_hInstance);
    m_hInstance = nullptr;
  }
}

bool Window::processMessages()
{
  MSG msg = {};
  while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
  {
    if (msg.message == WM_QUIT)
    {
      return false; // terminar loop principal
    }

    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return true;
}
