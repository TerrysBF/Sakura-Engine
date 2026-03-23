#pragma once
#include "Prerequisites.h"
#include "InputLayout.h"

class Device;
class DeviceContext;
class LayoutBuilder;

/**
 * @class ShaderProgram
 * @brief Es la "pareja" de programas (Vertex y Pixel Shader) que le dicen a la tarjeta de video cómo dibujar.
 * * Vertex Shader mueve los puntos (vértices) y el Pixel Shader elige el color.
 * Esta clase se encarga de cargarlos, compilarlos y activarlos cuando vamos a dibujar algo.
 */
class
  ShaderProgram {
public:
  /**
   * @brief Constructor por defecto.
   */
  ShaderProgram() = default;

  /**
   * @brief Destructor por defecto.
   * @details No libera automáticamente los recursos COM; llamar a destroy().
   */
  ~ShaderProgram() = default;

  /**
   * @brief Carga y prepara los Shaders .
   * @param device El "creador" de recursos de la tarjeta de video.
   * @param layoutBuilder El plano que explica cómo están organizados los datos de cada punto.
   * @return S_OK si todo salió bien.
   */
  HRESULT
    init(Device& device, const std::string& fileName, LayoutBuilder layoutBuilder);

  void
    update();

  void
    render(DeviceContext& deviceContext);

  void
    render(DeviceContext& deviceContext, ShaderType type);

  void
    destroy();

  /**
   * @brief Crea un Input Layout asociado al Vertex Shader.
   *
   * @param device Dispositivo con el que se creará el recurso.
   * @param Layout Descripción de los elementos de entrada.
   * @return @c S_OK si fue exitoso; código @c HRESULT en caso de error.
   */
  HRESULT
    CreateInputLayout(Device& device, LayoutBuilder);

  /**
   * @brief Crea un shader (Vertex o Pixel) a partir del archivo establecido en @c m_shaderFileName.
   *
   * @param device Dispositivo con el que se creará el recurso.
   * @param type   Tipo de shader a crear.
   * @return @c S_OK si fue exitoso; código @c HRESULT en caso de error.
   */
  HRESULT
    CreateShader(Device& device, ShaderType type);

  /**
   * @brief Crea un shader (Vertex o Pixel) a partir de un archivo HLSL.
   *
   * @param device   Dispositivo con el que se creará el recurso.
   * @param type     Tipo de shader a crear.
   * @param fileName Nombre del archivo HLSL.
   * @return @c S_OK si fue exitoso; código @c HRESULT en caso de error.
   */
  HRESULT
    CreateShader(Device& device, ShaderType type, const std::string& fileName);

  /**
   * @brief Traduce el código HLSL (que entendemos nosotros) a lenguaje de máquina.
   * @param szFileName Ruta del archivo.
   * @param szEntryPoint El nombre de la función principal en el código HLSL.
   * @param szShaderModel La versión de DirectX10.
   * @param ppBlobOut Aquí se guarda el código binario resultante.
   */
  HRESULT
    CompileShaderFromFile(char* szFileName,
      LPCSTR szEntryPoint,
      LPCSTR szShaderModel,
      ID3DBlob** ppBlobOut);

public:
  /**
   * @brief Vertex Shader compilado y creado en GPU.
   */
  ID3D11VertexShader* m_VertexShader = nullptr;

  /**
   * @brief Pixel Shader compilado y creado en GPU.
   */
  ID3D11PixelShader* m_PixelShader = nullptr;

  /**
   * @brief Input Layout asociado al Vertex Shader.
   */
  InputLayout m_inputLayout;

private:
  /**
   * @brief Nombre del archivo HLSL asociado a este programa de shaders.
   */
  std::string m_shaderFileName;

  /**
   * @brief Bytecode compilado del Vertex Shader.
   */
  ID3DBlob* m_vertexShaderData = nullptr;

  /**
   * @brief Bytecode compilado del Pixel Shader.
   */
  ID3DBlob* m_pixelShaderData = nullptr;
};