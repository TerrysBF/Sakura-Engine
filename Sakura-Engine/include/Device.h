#pragma once
#include "Prerequisites.h"

/**
 * @class Device
 * @brief Clase que envuelve un ID3D11Device.
 *
 * Se usa para crear y manejar recursos básicos de Direct3D 11
 * como texturas, buffers, shaders y estados.
 *
 * Ojo: solo controla el device, no el context.
 */
class Device {
public:
  Device() = default;
  ~Device() = default;

  /// Inicializa el device.
  void init();

  /// Aquí irían actualizaciones (placeholder).
  void update();

  /// Aquí iría la parte de dibujado (placeholder).
  void render();

  /// Libera el device.
  void destroy();

  /**
   * @brief Crea una Render Target View.
   * @param pResource Recurso base (ej. textura).
   * @param pDesc Descriptor (o nullptr por defecto).
   * @param ppRTView Puntero de salida.
   */
  HRESULT CreateRenderTargetView(ID3D11Resource* pResource,
    const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
    ID3D11RenderTargetView** ppRTView);

  /**
   * @brief Crea una textura 2D.
   * @param pDesc Configuración de la textura.
   * @param pInitialData Datos iniciales (opcional).
   * @param ppTexture2D Puntero de salida.
   */
  HRESULT CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc,
    const D3D11_SUBRESOURCE_DATA* pInitialData,
    ID3D11Texture2D** ppTexture2D);

  /**
   * @brief Crea una Depth Stencil View.
   * @param pResource Textura de depth/stencil.
   * @param pDesc Descriptor (opcional).
   * @param ppDepthStencilView Puntero de salida.
   */
  HRESULT CreateDepthStencilView(ID3D11Resource* pResource,
    const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
    ID3D11DepthStencilView** ppDepthStencilView);

  /**
   * @brief Crea un Vertex Shader.
   * @param pShaderBytecode Código compilado.
   * @param BytecodeLength Tamaño del código.
   * @param pClassLinkage Opcional.
   * @param ppVertexShader Salida del shader.
   */
  HRESULT CreateVertexShader(const void* pShaderBytecode,
    unsigned int BytecodeLength,
    ID3D11ClassLinkage* pClassLinkage,
    ID3D11VertexShader** ppVertexShader);

  /**
   * @brief Crea un Input Layout.
   * @param pInputElementDescs Descriptores de entrada.
   * @param NumElements Cantidad de entradas.
   * @param pShaderBytecodeWithInputSignature Shader con la firma.
   * @param BytecodeLength Tamaño del código.
   * @param ppInputLayout Puntero de salida.
   */
  HRESULT CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
    unsigned int NumElements,
    const void* pShaderBytecodeWithInputSignature,
    unsigned int BytecodeLength,
    ID3D11InputLayout** ppInputLayout);

  /**
   * @brief Crea un Pixel Shader.
   * @param pShaderBytecode Código compilado.
   * @param BytecodeLength Tamaño del código.
   * @param pClassLinkage Opcional.
   * @param ppPixelShader Salida del shader.
   */
  HRESULT CreatePixelShader(const void* pShaderBytecode,
    unsigned int BytecodeLength,
    ID3D11ClassLinkage* pClassLinkage,
    ID3D11PixelShader** ppPixelShader);

  /**
   * @brief Crea un Buffer (vertex, index, constant, etc.).
   * @param pDesc Configuración del buffer.
   * @param pInitialData Datos iniciales (opcional).
   * @param ppBuffer Salida del buffer.
   */
  HRESULT CreateBuffer(const D3D11_BUFFER_DESC* pDesc,
    const D3D11_SUBRESOURCE_DATA* pInitialData,
    ID3D11Buffer** ppBuffer);

  /**
   * @brief Crea un Sampler State.
   * @param pSamplerDesc Configuración del sampler.
   * @param ppSamplerState Salida del estado.
   */
  HRESULT CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc,
    ID3D11SamplerState** ppSamplerState);

public:
  /**
   * @brief Puntero al device de D3D11.
   * @details Se crea en init() y se libera en destroy().
   */
  ID3D11Device* m_device = nullptr;
};
