#pragma once
#include "Prerequisites.h"

class Window;
class DeviceContext;

// Clase sencilla que envuelve un D3D11_VIEWPORT.
// Un viewport define en qué parte del render target se dibuja (tamaño y posición).
class Viewport {
public:
  // Constructor por defecto.
  Viewport() = default;

  // Destructor por defecto.
  ~Viewport() = default;

  // Inicializa el viewport usando el tamaño de la ventana.
  // Usa el ancho y alto del área cliente de la ventana.
  HRESULT init(const Window& window);

  // Inicializa el viewport con un ancho y alto específicos.
  // La profundidad va de 0.0f a 1.0f por defecto.
  HRESULT init(unsigned int width, unsigned int height);

  // Update vacío por ahora (se podría usar si hay resize dinámico).
  void update();

  // Aplica el viewport al contexto de D3D11.
  // Llama a RSSetViewports con m_viewport.
  void render(DeviceContext& deviceContext);

  // No hay recursos COM que liberar, así que destroy está vacío.
  void destroy() {}

public:
  // Estructura de Direct3D que guarda los datos del viewport
  // (X, Y, Width, Height, MinDepth, MaxDepth).
  D3D11_VIEWPORT m_viewport;
};
