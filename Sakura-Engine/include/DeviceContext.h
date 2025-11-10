#pragma once
#include "Prerequisites.h"

/*
 * Clase DeviceContext
 *
 * Envuelve el ID3D11DeviceContext (el contexto inmediato de D3D11).
 * Con este contexto se hacen casi todas las llamadas de render:
 * setear viewports, buffers, shaders y hacer los draws.
 */
class DeviceContext {
public:
  DeviceContext() = default;
  ~DeviceContext() = default;

  // Aquí se podría inicializar el contexto si hiciera falta (placeholder)
  void init();

  // Placeholder de actualización (por ahora no hace nada)
  void update();

  // Placeholder de render (para pruebas si se necesita)
  void render();

  // Libera el contexto inmediato (Release sobre m_deviceContext)
  void destroy();

  // Configura los viewports usados por el rasterizer
  // NumViewports -> cuántos viewports
  // pViewports   -> arreglo con la info de cada viewport
  void RSSetViewports(unsigned int NumViewports, const D3D11_VIEWPORT* pViewports);

  // Enlaza shader resource views en el Pixel Shader
  // StartSlot -> primer slot
  // NumViews  -> cuántas SRVs
  // ppShaderResourceViews -> arreglo de SRVs
  void PSSetShaderResources(unsigned int StartSlot,
    unsigned int NumViews,
    ID3D11ShaderResourceView* const* ppShaderResourceViews);

  // Activa un Input Layout en el Input Assembler
  void IASetInputLayout(ID3D11InputLayout* pInputLayout);

  // Asigna el Vertex Shader al pipeline
  // ppClassInstances y NumClassInstances casi siempre se dejan en 0/nullptr
  void VSSetShader(ID3D11VertexShader* pVertexShader,
    ID3D11ClassInstance* const* ppClassInstances,
    unsigned int NumClassInstances);

  // Asigna el Pixel Shader al pipeline
  void PSSetShader(ID3D11PixelShader* pPixelShader,
    ID3D11ClassInstance* const* ppClassInstances,
    unsigned int NumClassInstances);

  // Sube datos desde CPU a un recurso de GPU
  // Normalmente se usa con buffers o texturas usando UpdateSubresource
  void UpdateSubresource(ID3D11Resource* pDstResource,
    unsigned int DstSubresource,
    const D3D11_BOX* pDstBox,
    const void* pSrcData,
    unsigned int SrcRowPitch,
    unsigned int SrcDepthPitch);

  // Asigna uno o varios vertex buffers al Input Assembler
  // StartSlot      -> primer slot
  // NumBuffers     -> cuántos buffers
  // ppVertexBuffers-> arreglo de buffers
  // pStrides       -> tamaño de cada vértice por buffer
  // pOffsets       -> offset inicial en cada buffer
  void IASetVertexBuffers(unsigned int StartSlot,
    unsigned int NumBuffers,
    ID3D11Buffer* const* ppVertexBuffers,
    const unsigned int* pStrides,
    const unsigned int* pOffsets);

  // Asigna el index buffer al Input Assembler
  // Format -> formato de índice (R16_UINT o R32_UINT)
  // Offset -> desplazamiento inicial en bytes
  void IASetIndexBuffer(ID3D11Buffer* pIndexBuffer,
    DXGI_FORMAT Format,
    unsigned int Offset);

  // Asigna samplers al Pixel Shader
  // Sirve para controlar el filtrado y el modo de dirección de las texturas
  void PSSetSamplers(unsigned int StartSlot,
    unsigned int NumSamplers,
    ID3D11SamplerState* const* ppSamplers);

  // Setea el estado de rasterización (culling, wireframe/solid, etc.)
  void RSSetState(ID3D11RasterizerState* pRasterizerState);

  // Setea el estado de blending en la etapa OM
  // BlendFactor -> color usado en algunas operaciones de blending
  // SampleMask  -> máscara de muestras (normalmente 0xFFFFFFFF)
  void OMSetBlendState(ID3D11BlendState* pBlendState,
    const float BlendFactor[4],
    unsigned int SampleMask);

  // Enlaza render target views y depth stencil view en el Output Merger
  // NumViews           -> cuántos RTVs
  // ppRenderTargetViews-> arreglo de RTVs
  // pDepthStencilView  -> DSV (puede ser nullptr si no se usa depth)
  void OMSetRenderTargets(unsigned int NumViews,
    ID3D11RenderTargetView* const* ppRenderTargetViews,
    ID3D11DepthStencilView* pDepthStencilView);

  // Define la topología de primitivas en el Input Assembler
  // Ejemplo: TRIANGLELIST, LINELIST, etc.
  void IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);

  // Limpia un render target con un color específico
  void ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView,
    const float ColorRGBA[4]);

  // Limpia la vista de profundidad/esténcil
  // ClearFlags -> D3D11_CLEAR_DEPTH, D3D11_CLEAR_STENCIL o ambos
  // Depth      -> valor para el buffer de profundidad (normalmente 1.0f)
  // Stencil    -> valor para el buffer de stencil (normalmente 0)
  void ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView,
    unsigned int ClearFlags,
    float Depth,
    UINT8 Stencil);

  // Enlaza constant buffers al Vertex Shader
  void VSSetConstantBuffers(unsigned int StartSlot,
    unsigned int NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers);

  // Enlaza constant buffers al Pixel Shader
  void PSSetConstantBuffers(unsigned int StartSlot,
    unsigned int NumBuffers,
    ID3D11Buffer* const* ppConstantBuffers);

  // Dibuja usando index buffer
  // IndexCount        -> cuántos índices se van a usar
  // StartIndexLocation-> índice inicial dentro del index buffer
  // BaseVertexLocation-> offset que se suma a los índices (normalmente 0)
  void DrawIndexed(unsigned int IndexCount,
    unsigned int StartIndexLocation,
    int BaseVertexLocation);

public:
  // Contexto inmediato de D3D11
  // Lo usa todo el pipeline para hacer las llamadas de dibujo
  ID3D11DeviceContext* m_deviceContext = nullptr;
};
