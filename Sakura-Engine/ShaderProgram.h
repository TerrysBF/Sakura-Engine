#pragma once
#include "Prerequisites.h"
#include "InputLayout.h"

class Device;
class DeviceContext;

// Clase que maneja un vertex shader, un pixel shader y su input layout.
// Se encarga de compilar los shaders desde archivo, crearlos en el device
// y luego ponerlos en el pipeline para dibujar.
class ShaderProgram {
public:
  // Constructor por defecto, no crea nada todavía.
  ShaderProgram() = default;

  // Destructor por defecto.
  // No libera nada por sí solo, para eso está destroy().
  ~ShaderProgram() = default;

  // Inicializa el programa de shaders desde un archivo HLSL.
  // Compila y crea el VS y el PS usando el archivo que se pasa.
  // También crea el input layout usando la descripción de los vértices.
  // Devuelve S_OK si todo va bien o un HRESULT de error.
  HRESULT init(Device& device,
    const std::string& fileName,
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout);

  // Función de actualización (por ahora no hace nada).
  // Se podría usar para recargar shaders en caliente más adelante.
  void update();

  // Activa el vertex shader, el pixel shader y el input layout en el pipeline.
  // Llama a VSSetShader, PSSetShader y también setea el input layout
  // en el device context.
  void render(DeviceContext& deviceContext);

  // Activa solo un shader específico en el pipeline.
  // Sirve para poner solo el VS o solo el PS usando el enum ShaderType.
  void render(DeviceContext& deviceContext, ShaderType type);

  // Libera todos los recursos del shader program:
  // vertex shader, pixel shader, input layout y blobs de bytecode.
  void destroy();

  // Crea el input layout usando la info del vertex shader.
  // Devuelve S_OK si se creó bien o un HRESULT de error.
  HRESULT CreateInputLayout(Device& device,
    std::vector<D3D11_INPUT_ELEMENT_DESC> Layout);

  // Crea un shader (VS o PS) usando el archivo guardado en m_shaderFileName.
  // Compila el shader desde ese archivo y luego crea el objeto en GPU.
  HRESULT CreateShader(Device& device, ShaderType type);

  // Crea un shader (VS o PS) usando el archivo que se pasa por parámetro.
  // También guarda el nombre del archivo en m_shaderFileName.
  HRESULT CreateShader(Device& device, ShaderType type, const std::string& fileName);

  // Compila un shader desde un archivo HLSL.
  // Llama a la función de compilación con entry point y shader model.
  HRESULT CompileShaderFromFile(char* szFileName,
    LPCSTR szEntryPoint,
    LPCSTR szShaderModel,
    ID3DBlob** ppBlobOut);

public:
  // Puntero al vertex shader creado en GPU.
  ID3D11VertexShader* m_VertexShader = nullptr;

  // Puntero al pixel shader creado en GPU.
  ID3D11PixelShader* m_PixelShader = nullptr;

  // Input layout que describe cómo leen los vértices el vertex shader.
  InputLayout m_inputLayout;

private:
  // Nombre del archivo de shader que se está usando.
  std::string m_shaderFileName;

  // Bytecode compilado del vertex shader.
  // Se usa para crear el VS y también para crear el input layout.
  ID3DBlob* m_vertexShaderData = nullptr;

  // Bytecode compilado del pixel shader.
  ID3DBlob* m_pixelShaderData = nullptr;
};
