#pragma once
#include "Prerequisites.h"

/*
 * Clase Device
 *
 * Envuelve el ID3D11Device de Direct3D.
 * Con este objeto se crean casi todos los recursos de la GPU:
 * texturas, buffers, shaders, samplers, etc.
 * Aquí no se maneja el DeviceContext, solo el device.
 */
class Device {
public:
  // Constructor y destructor vacíos (solo se maneja el puntero m_device)
  Device() = default;
  ~Device() = default;

  // Aquí se podría crear el device si se necesitara (placeholder)
  void init();

  // Placeholder de update (no hace nada por ahora)
  void update();

  // Placeholder de render (no hace nada por ahora)
  void render();

  // Libera el ID3D11Device si está creado
  void destroy();

  /*
   * Crea una Render Target View (RTV).
   *
   * pResource -> recurso base (normalmente una textura del back buffer)
   * pDesc    -> descriptor de la RTV (o nullptr para valores por defecto)
   * ppRTView -> puntero de salida donde se guarda la RTV
   */
  HRESULT CreateRenderTargetView(ID3D11Resource* pResource,
    const D3D11_RENDER_TARGET_VIEW_DESC* pDesc,
    ID3D11RenderTargetView** ppRTView);

  /*
   * Crea una textura 2D.
   *
   * pDesc        -> descripción de la textura (tamaño, formato, etc.)
   * pInitialData -> datos iniciales (puede ser nullptr si solo se reserva memoria)
   * ppTexture2D  -> puntero de salida con la textura creada
   */
  HRESULT CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc,
    const D3D11_SUBRESOURCE_DATA* pInitialData,
    ID3D11Texture2D** ppTexture2D);

  /*
   * Crea una Depth Stencil View (DSV).
   *
   * pResource          -> textura de depth/stencil
   * pDesc              -> descripción de la DSV (formato, dimensión, etc.)
   * ppDepthStencilView -> puntero de salida con la DSV
   */
  HRESULT CreateDepthStencilView(ID3D11Resource* pResource,
    const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc,
    ID3D11DepthStencilView** ppDepthStencilView);

  /*
   * Crea un Vertex Shader.
   *
   * pShaderBytecode -> código compilado del shader (bytecode)
   * BytecodeLength  -> tamaño del bytecode en bytes
   * pClassLinkage   -> opcional, casi siempre nullptr
   * ppVertexShader  -> puntero de salida con el VS
   */
  HRESULT CreateVertexShader(const void* pShaderBytecode,
    unsigned int BytecodeLength,
    ID3D11ClassLinkage* pClassLinkage,
    ID3D11VertexShader** ppVertexShader);

  /*
   * Crea un Input Layout.
   *
   * pInputElementDescs -> arreglo con la descripción de los elementos de entrada (POSITION, TEXCOORD, etc.)
   * NumElements        -> número de elementos en el arreglo anterior
   * pShaderBytecodeWithInputSignature -> bytecode del VS que contiene la firma de entrada
   * BytecodeLength     -> tamaño del bytecode
   * ppInputLayout      -> puntero de salida con el layout creado
   */
  HRESULT CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
    unsigned int NumElements,
    const void* pShaderBytecodeWithInputSignature,
    unsigned int BytecodeLength,
    ID3D11InputLayout** ppInputLayout);

  /*
   * Crea un Pixel Shader.
   *
   * pShaderBytecode -> código compilado del shader (bytecode)
   * BytecodeLength  -> tamaño del bytecode en bytes
   * pClassLinkage   -> opcional, casi siempre nullptr
   * ppPixelShader   -> puntero de salida con el PS
   */
  HRESULT CreatePixelShader(const void* pShaderBytecode,
    unsigned int BytecodeLength,
    ID3D11ClassLinkage* pClassLinkage,
    ID3D11PixelShader** ppPixelShader);

  /*
   * Crea un Buffer genérico (vertex, index, constant, etc.).
   *
   * pDesc        -> descripción del buffer (tamaño, tipo de uso, bind flags)
   * pInitialData -> datos iniciales (puede ser nullptr para buffer vacío)
   * ppBuffer     -> puntero de salida con el buffer creado
   */
  HRESULT CreateBuffer(const D3D11_BUFFER_DESC* pDesc,
    const D3D11_SUBRESOURCE_DATA* pInitialData,
    ID3D11Buffer** ppBuffer);

  /*
   * Crea un Sampler State.
   *
   * pSamplerDesc   -> descripción del sampler (filtro, modo de direcciones, etc.)
   * ppSamplerState -> puntero de salida con el sampler creado
   */
  HRESULT CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc,
    ID3D11SamplerState** ppSamplerState);

public:
  // Puntero al device de Direct3D 11
  // Se inicializa al crear el device y se libera en destroy()
  ID3D11Device* m_device = nullptr;
};
