#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;

/*
 * Clase InputLayout
 *
 * Representa el ID3D11InputLayout, que dice cómo están acomodados
 * los datos del vértice en el vertex buffer (posición, uv, etc.)
 * y cómo se mandan al Vertex Shader.
 */
class InputLayout {
public:
  // Constructor vacío, no crea nada todavía
  InputLayout() = default;

  // Destructor vacío, la liberación se hace en destroy()
  ~InputLayout() = default;

  // Inicializa el input layout usando:
  // - el device
  // - un vector con la descripción de cada atributo del vértice
  // - el bytecode del Vertex Shader (para la firma de entrada)
  //
  // Si sale bien, m_inputLayout apunta a un layout válido.
  HRESULT init(Device& device,
    std::vector<D3D11_INPUT_ELEMENT_DESC>& Layout,
    ID3DBlob* VertexShaderData);

  // Placeholder por si algún día se quiere actualizar algo del layout
  // Ahorita no hace nada.
  void update();

  // Activa el input layout en el pipeline
  // (llama internamente a IASetInputLayout en el deviceContext)
  void render(DeviceContext& deviceContext);

  // Libera el recurso de input layout
  // Después de esto, m_inputLayout queda en nullptr.
  void destroy();

public:
  // Puntero al input layout de D3D11
  // Se crea en init() y se libera en destroy()
  ID3D11InputLayout* m_inputLayout = nullptr;
};
