/**
 * @file DeviceContext.h
 * @brief Declara la API de DeviceContext dentro del subsistema Core.
 * @ingroup core
 */
#pragma once
#include "Prerequisites.h"

class
  DeviceContext {
public:
  DeviceContext() = default;
  ~DeviceContext() = default;

  /**
   * @brief Inicializa el contexto del dispositivo.
   *
   * Este m?todo se utiliza como punto de inicializaci?n; la implementaci?n
   * puede asociar el contexto inmediato creado junto con @c ID3D11Device.
   */
  void
    init();

  /**
   * @brief Actualiza par?metros internos del contexto.
   * @note M?todo placeholder, ?til para extender funcionalidades.
   */
  void
    update();

  /**
   * @brief Ejecuta operaciones relacionadas con render.
   * @note M?todo placeholder; puede ser usado para depuraci?n.
   */
  void
    render();

  /**
   * @brief Libera el recurso @c ID3D11DeviceContext.
   *
   * @post @c m_deviceContext == nullptr.
   */
  void
    destroy();

  /**
   * @brief Configura los viewports en la etapa de rasterizaci?n.
   *
   * @param NumViewports N?mero de viewports.
   * @param pViewports   Puntero a un arreglo de descriptores de viewport.
   */
  void
    RSSetViewports(unsigned int NumViewports, const D3D11_VIEWPORT* pViewports);

  /**
   * @brief Asigna Shader Resource Views a la etapa de Pixel Shader.
   *
   * @param StartSlot             Slot inicial.
   * @param NumViews              N?mero de vistas a asignar.
   * @param ppShaderResourceViews Arreglo de vistas de recurso de shader.
   */
  void
    PSSetShaderResources(unsigned int StartSlot,
      unsigned int NumViews,
      ID3D11ShaderResourceView* const* ppShaderResourceViews);

  /**
   * @brief Define el Input Layout activo en la etapa de ensamblado de entrada.
   *
   * @param pInputLayout Input layout a asignar.
   */
  void
    IASetInputLayout(ID3D11InputLayout* pInputLayout);

  /**
   * @brief Asigna un Vertex Shader al pipeline.
   *
   * @param pVertexShader   Puntero al vertex shader.
   * @param ppClassInstances Instancias de clase (opcional).
   * @param NumClassInstances N?mero de instancias de clase.
   */
  void
    VSSetShader(ID3D11VertexShader* pVertexShader,
      ID3D11ClassInstance* const* ppClassInstances,
      unsigned int NumClassInstances);

  /**
   * @brief Asigna un Pixel Shader al pipeline.
   *
   * @param pPixelShader     Puntero al pixel shader.
   * @param ppClassInstances Instancias de clase (opcional).
   * @param NumClassInstances N?mero de instancias de clase.
   */
  void
    PSSetShader(ID3D11PixelShader* pPixelShader,
      ID3D11ClassInstance* const* ppClassInstances,
      unsigned int NumClassInstances);

  /**
   * @brief Copia datos desde CPU hacia un recurso en GPU.
   *
   * @param pDstResource   Recurso destino.
   * @param DstSubresource ?ndice de subrecurso.
   * @param pDstBox        Regi?n destino (puede ser nullptr).
   * @param pSrcData       Datos fuente.
   * @param SrcRowPitch    Tama?o por fila.
   * @param SrcDepthPitch  Tama?o por capa de profundidad.
   */
  void
    UpdateSubresource(ID3D11Resource* pDstResource,
      unsigned int DstSubresource,
      const D3D11_BOX* pDstBox,
      const void* pSrcData,
      unsigned int SrcRowPitch,
      unsigned int SrcDepthPitch);

  /**
   * @brief Asigna buffers de v?rtices a la etapa de ensamblado de entrada.
   *
   * @param StartSlot       Slot inicial.
   * @param NumBuffers      N?mero de buffers.
   * @param ppVertexBuffers Arreglo de punteros a vertex buffers.
   * @param pStrides        Arreglo con tama?os de cada v?rtice.
   * @param pOffsets        Arreglo con offsets iniciales.
   */
  void
    IASetVertexBuffers(unsigned int StartSlot,
      unsigned int NumBuffers,
      ID3D11Buffer* const* ppVertexBuffers,
      const unsigned int* pStrides,
      const unsigned int* pOffsets);

  /**
   * @brief Asigna un Index Buffer a la etapa de ensamblado de entrada.
   *
   * @param pIndexBuffer Buffer de ?ndices.
   * @param Format       Formato de ?ndice (ej. DXGI_FORMAT_R16_UINT).
   * @param Offset       Offset inicial en bytes.
   */
  void
    IASetIndexBuffer(ID3D11Buffer* pIndexBuffer,
      DXGI_FORMAT Format,
      unsigned int Offset);

  /**
   * @brief Asigna Sampler States a la etapa de Pixel Shader.
   *
   * @param StartSlot   Slot inicial.
   * @param NumSamplers N?mero de samplers.
   * @param ppSamplers  Arreglo de sampler states.
   */
  void
    PSSetSamplers(unsigned int StartSlot,
      unsigned int NumSamplers,
      ID3D11SamplerState* const* ppSamplers);

  /**
   * @brief Configura el Rasterizer State actual.
   *
   * @param pRasterizerState Estado de rasterizaci?n.
   */
  void
    RSSetState(ID3D11RasterizerState* pRasterizerState);

  /**
   * @brief Asigna un Blend State al Output Merger.
   *
   * @param pBlendState Estado de blending.
   * @param BlendFactor Factor de mezcla (RGBA).
   * @param SampleMask  M?scara de muestras.
   */
  void
    OMSetBlendState(ID3D11BlendState* pBlendState,
      const float BlendFactor[4],
      unsigned int SampleMask);

  /**
   * @brief Asigna Render Targets y Depth Stencil al Output Merger.
   *
   * @param NumViews            N?mero de render targets.
   * @param ppRenderTargetViews Arreglo de render target views.
   * @param pDepthStencilView   Vista de profundidad/est?ncil.
   */
  void
    OMSetRenderTargets(unsigned int NumViews,
      ID3D11RenderTargetView* const* ppRenderTargetViews,
      ID3D11DepthStencilView* pDepthStencilView);

  /**
   * @brief Define la topolog?a de primitivas a renderizar.
   *
   * @param Topology Tipo de topolog?a (ej. D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST).
   */
  void
    IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);

  /**
   * @brief Limpia un Render Target con un color dado.
   *
   * @param pRenderTargetView Render target a limpiar.
   * @param ColorRGBA         Color en formato RGBA.
   */
  void
    ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView,
      const float ColorRGBA[4]);

  /**
   * @brief Limpia un Depth Stencil View.
   *
   * @param pDepthStencilView Vista a limpiar.
   * @param ClearFlags        Flags de limpieza (D3D11_CLEAR_DEPTH / D3D11_CLEAR_STENCIL).
   * @param Depth             Valor de profundidad (0.0 - 1.0).
   * @param Stencil           Valor de est?ncil.
   */
  void
    ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView,
      unsigned int ClearFlags,
      float Depth,
      UINT8 Stencil);

  /**
   * @brief Asigna constant buffers a la etapa de Vertex Shader.
   *
   * @param StartSlot       Slot inicial.
   * @param NumBuffers      N?mero de buffers.
   * @param ppConstantBuffers Arreglo de constant buffers.
   */
  void
    VSSetConstantBuffers(unsigned int StartSlot,
      unsigned int NumBuffers,
      ID3D11Buffer* const* ppConstantBuffers);

  /**
   * @brief Asigna constant buffers a la etapa de Pixel Shader.
   *
   * @param StartSlot       Slot inicial.
   * @param NumBuffers      N?mero de buffers.
   * @param ppConstantBuffers Arreglo de constant buffers.
   */
  void
    PSSetConstantBuffers(unsigned int StartSlot,
      unsigned int NumBuffers,
      ID3D11Buffer* const* ppConstantBuffers);

  /**
   * @brief Env?a un comando de dibujado de primitivas indexadas.
   *
   * @param IndexCount         N?mero de ?ndices a renderizar.
   * @param StartIndexLocation Posici?n inicial en el buffer de ?ndices.
   * @param BaseVertexLocation Offset aplicado a los v?rtices.
   */
  void
    DrawIndexed(unsigned int IndexCount,
      unsigned int StartIndexLocation,
      int BaseVertexLocation);
public:
  /**
   * @brief Puntero al contexto inmediato de Direct3D 11.
   * @details V?lido tras init(); liberado en destroy().
   */
  ID3D11DeviceContext* m_deviceContext = nullptr;

};

