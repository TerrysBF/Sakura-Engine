#pragma once
#include "Prerequisites.h"

class
  DeviceContext {
public:
  DeviceContext() = default;
  ~DeviceContext() = default;

  /**
   * @brief Inicializa el contexto (obtiene/ata el inmediato).
   */
  void
    init();

  /**
   * @brief Placeholder de actualización (por ahora sin uso).
   */
  void
    update();

  /**
   * @brief Placeholder de render (útil para pruebas).
   */
  void
    render();

  /**
   * @brief Libera el ID3D11DeviceContext.
   * @post m_deviceContext == nullptr
   */
  void
    destroy();

  /**
   * @brief Setea los viewports del rasterizer.
   * @param NumViewports cantidad de viewports
   * @param pViewports   arreglo de viewports
   */
  void
    RSSetViewports(unsigned int NumViewports, const D3D11_VIEWPORT* pViewports);

  /**
   * @brief Pone SRVs en el Pixel Shader.
   * @param StartSlot slot inicial
   * @param NumViews  cuántas vistas
   * @param ppShaderResourceViews arreglo de SRVs
   */
  void
    PSSetShaderResources(unsigned int StartSlot,
      unsigned int NumViews,
      ID3D11ShaderResourceView* const* ppShaderResourceViews);

  /**
   * @brief Activa un Input Layout en IA.
   */
  void
    IASetInputLayout(ID3D11InputLayout* pInputLayout);

  /**
   * @brief Setea el Vertex Shader.
   * @param pVertexShader shader
   * @param ppClassInstances instancias (opcional)
   * @param NumClassInstances cantidad de instancias
   */
  void
    VSSetShader(ID3D11VertexShader* pVertexShader,
      ID3D11ClassInstance* const* ppClassInstances,
      unsigned int NumClassInstances);

  /**
   * @brief Setea el Pixel Shader.
   * @param pPixelShader shader
   * @param ppClassInstances instancias (opcional)
   * @param NumClassInstances cantidad de instancias
   */
  void
    PSSetShader(ID3D11PixelShader* pPixelShader,
      ID3D11ClassInstance* const* ppClassInstances,
      unsigned int NumClassInstances);

  /**
   * @brief Sube datos de CPU a un recurso de GPU.
   */
  void
    UpdateSubresource(ID3D11Resource* pDstResource,
      unsigned int DstSubresource,
      const D3D11_BOX* pDstBox,
      const void* pSrcData,
      unsigned int SrcRowPitch,
      unsigned int SrcDepthPitch);

  /**
   * @brief Asigna vertex buffers a IA.
   * @param StartSlot slot inicial
   * @param NumBuffers cuántos buffers
   * @param ppVertexBuffers arreglo de VBs
   * @param pStrides stride por VB
   * @param pOffsets offset por VB
   */
  void
    IASetVertexBuffers(unsigned int StartSlot,
      unsigned int NumBuffers,
      ID3D11Buffer* const* ppVertexBuffers,
      const unsigned int* pStrides,
      const unsigned int* pOffsets);

  /**
   * @brief Asigna el index buffer a IA.
   * @param pIndexBuffer buffer de índices
   * @param Format formato (ej. R16_UINT)
   * @param Offset offset en bytes
   */
  void
    IASetIndexBuffer(ID3D11Buffer* pIndexBuffer,
      DXGI_FORMAT Format,
      unsigned int Offset);

  /**
   * @brief Setea samplers en el Pixel Shader.
   */
  void
    PSSetSamplers(unsigned int StartSlot,
      unsigned int NumSamplers,
      ID3D11SamplerState* const* ppSamplers);

  /**
   * @brief Setea el Rasterizer State actual.
   */
  void
    RSSetState(ID3D11RasterizerState* pRasterizerState);

  /**
   * @brief Setea el Blend State en OM.
   * @param pBlendState estado
   * @param BlendFactor RGBA
   * @param SampleMask máscara
   */
  void
    OMSetBlendState(ID3D11BlendState* pBlendState,
      const float BlendFactor[4],
      unsigned int SampleMask);

  /**
   * @brief Setea RTVs y DSV en OM.
   * @param NumViews cuántos RTVs
   * @param ppRenderTargetViews arreglo de RTVs
   * @param pDepthStencilView DSV
   */
  void
    OMSetRenderTargets(unsigned int NumViews,
      ID3D11RenderTargetView* const* ppRenderTargetViews,
      ID3D11DepthStencilView* pDepthStencilView);

  /**
   * @brief Define la topología en IA (triángulos, líneas, etc.).
   */
  void
    IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);

  /**
   * @brief Limpia un RTV con un color.
   */
  void
    ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView,
      const float ColorRGBA[4]);

  /**
   * @brief Limpia el DSV (depth/stencil).
   * @param ClearFlags D3D11_CLEAR_DEPTH/STENCIL
   * @param Depth valor de depth
   * @param Stencil valor de stencil
   */
  void
    ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView,
      unsigned int ClearFlags,
      float Depth,
      UINT8 Stencil);

  /**
   * @brief Setea constant buffers en VS.
   */
  void
    VSSetConstantBuffers(unsigned int StartSlot,
      unsigned int NumBuffers,
      ID3D11Buffer* const* ppConstantBuffers);

  /**
   * @brief Setea constant buffers en PS.
   */
  void
    PSSetConstantBuffers(unsigned int StartSlot,
      unsigned int NumBuffers,
      ID3D11Buffer* const* ppConstantBuffers);

  /**
   * @brief Dibuja usando índices.
   * @param IndexCount cuántos índices
   * @param StartIndexLocation inicio en el IB
   * @param BaseVertexLocation offset de vértices
   */
  void
    DrawIndexed(unsigned int IndexCount,
      unsigned int StartIndexLocation,
      int BaseVertexLocation);
public:
  /**
   * @brief Contexto inmediato de D3D11.
   * @details Válido tras init(); se libera en destroy().
   */
  ID3D11DeviceContext* m_deviceContext = nullptr;

};
