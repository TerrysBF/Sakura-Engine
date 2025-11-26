#pragma once
#include "Prerequisites.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include <string>

// Puedes adelantar las declaraciones si quieres, pero con Prerequisites.h
// normalmente ya viene d3d11.h incluido.
// struct ID3D11Device;
// struct ID3D11DeviceContext;

class UserInterface
{
public:
  UserInterface();
  ~UserInterface();

  // Inicializa ImGui con la ventana de Win32 y el dispositivo de D3D11
  void init(void* window, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

  // Lógica por frame (inputs, etc.)
  void update();

  // Dibuja toda la UI ImGui en el backbuffer actual
  void render();

  // Libera el contexto de ImGui
  void destroy();

  // Pequeño control para editar un vec3 con un label
  void vec3Control(
    const std::string& label,
    float* values,
    float resetValue = 0.0f,
    float columnWidth = 100.0f
  );

private:
  bool m_initialized = false;
};
