// Implementación del contexto de dispositivo de D3D11.
// Aquí se hacen los binds, clears y draw usando ID3D11DeviceContext.

#include "DeviceContext.h"

// Libera el contexto inmediato de D3D11 almacenado en m_deviceContext.
void
DeviceContext::destroy() {
  SAFE_RELEASE(m_deviceContext);
}

// Configura los viewports que usará el rasterizer.
// NumViewports indica cuántos hay y pViewports es el arreglo con los datos.
void
DeviceContext::RSSetViewports(unsigned int NumViewports,
  const D3D11_VIEWPORT* pViewports) {
  if (!pViewports) {
    ERROR("DeviceContext", "RSSetViewports", "pViewports is nullptr");
    return;
  }

  m_deviceContext->RSSetViewports(NumViewports, pViewports);
}

// Enlaza Shader Resource Views (texturas, etc.) al Pixel Shader.
// StartSlot es el primer slot, NumViews es cuántas vistas se van a poner.
void
DeviceContext::PSSetShaderResources(unsigned int StartSlot,
  unsigned int NumViews,
  ID3D11ShaderResourceView* const* ppShaderResourceViews) {
  if (!ppShaderResourceViews) {
    ERROR("DeviceContext", "PSSetShaderResources", "ppShaderResourceViews is nullptr");
    return;
  }
  m_deviceContext->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

// Activa un Input Layout en la etapa de Input Assembler.
// Define cómo se leen los vértices del vertex buffer.
void
DeviceContext::IASetInputLayout(ID3D11InputLayout* pInputLayout) {
  if (!pInputLayout) {
    ERROR("DeviceContext", "IASetInputLayout", "pInputLayout is nullptr");
    return;
  }

  m_deviceContext->IASetInputLayout(pInputLayout);
}

// Asigna el Vertex Shader al pipeline.
// ppClassInstances y NumClassInstances casi siempre se dejan en nullptr y 0.
void
DeviceContext::VSSetShader(ID3D11VertexShader* pVertexShader,
  ID3D11ClassInstance* const* ppClassInstances,
  unsigned int NumClassInstances) {
  if (!pVertexShader) {
    ERROR("DeviceContext", "VSSetShader", "pVertexShader is nullptr");
    return;
  }
  m_deviceContext->VSSetShader(pVertexShader, ppClassInstances, NumClassInstances);
}

// Asigna el Pixel Shader al pipeline.
void
DeviceContext::PSSetShader(ID3D11PixelShader* pPixelShader,
  ID3D11ClassInstance* const* ppClassInstances,
  unsigned int NumClassInstances) {
  if (!pPixelShader) {
    ERROR("DeviceContext", "PSSetShader", "pPixelShader is nullptr");
    return;
  }
  m_deviceContext->PSSetShader(pPixelShader, ppClassInstances, NumClassInstances);
}

// Copia datos desde CPU hacia un recurso en la GPU.
// Se usa mucho para actualizar constant buffers.
void
DeviceContext::UpdateSubresource(ID3D11Resource* pDstResource,
  unsigned int DstSubresource,
  const D3D11_BOX* pDstBox,
  const void* pSrcData,
  unsigned int SrcRowPitch,
  unsigned int SrcDepthPitch) {
  if (!pDstResource || !pSrcData) {
    ERROR("DeviceContext", "UpdateSubresource",
      "Invalid arguments: pDstResource or pSrcData is nullptr");
    return;
  }
  m_deviceContext->UpdateSubresource(pDstResource,
    DstSubresource,
    pDstBox,
    pSrcData,
    SrcRowPitch,
    SrcDepthPitch);
}

// Enlaza uno o varios vertex buffers al Input Assembler.
// ppVertexBuffers tiene los buffers, pStrides el tamaño de cada vértice y pOffsets el offset inicial.
void
DeviceContext::IASetVertexBuffers(unsigned int StartSlot,
  unsigned int NumBuffers,
  ID3D11Buffer* const* ppVertexBuffers,
  const unsigned int* pStrides,
  const unsigned int* pOffsets) {
  if (!ppVertexBuffers || !pStrides || !pOffsets) {
    ERROR("DeviceContext", "IASetVertexBuffers",
      "Invalid arguments: ppVertexBuffers, pStrides, or pOffsets is nullptr");
    return;
  }
  m_deviceContext->IASetVertexBuffers(StartSlot,
    NumBuffers,
    ppVertexBuffers,
    pStrides,
    pOffsets);
}

// Enlaza un index buffer al Input Assembler.
// Format indica si los índices son de 16 bits o 32 bits.
void
DeviceContext::IASetIndexBuffer(ID3D11Buffer* pIndexBuffer,
  DXGI_FORMAT Format,
  unsigned int Offset) {
  if (!pIndexBuffer) {
    ERROR("DeviceContext", "IASetIndexBuffer", "pIndexBuffer is nullptr");
    return;
  }
  m_deviceContext->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}

// Enlaza samplers al Pixel Shader.
// Los samplers definen el filtrado y el modo de repetición de las texturas.
void
DeviceContext::PSSetSamplers(unsigned int StartSlot,
  unsigned int NumSamplers,
  ID3D11SamplerState* const* ppSamplers) {
  if (!ppSamplers) {
    ERROR("DeviceContext", "PSSetSamplers", "ppSamplers is nullptr");
    return;
  }
  m_deviceContext->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
}

// Cambia el estado de rasterización.
// Aquí se controla el culling, wireframe, etc.
void
DeviceContext::RSSetState(ID3D11RasterizerState* pRasterizerState) {
  if (!pRasterizerState) {
    ERROR("DeviceContext", "RSSetState", "pRasterizerState is nullptr");
    return;
  }
  m_deviceContext->RSSetState(pRasterizerState);
}

// Configura el estado de blending en la etapa de salida (Output Merger).
// BlendFactor y SampleMask controlan cómo se mezcla el color final.
void
DeviceContext::OMSetBlendState(ID3D11BlendState* pBlendState,
  const float BlendFactor[4],
  unsigned int SampleMask) {
  if (!pBlendState) {
    ERROR("DeviceContext", "OMSetBlendState", "pBlendState is nullptr");
    return;
  }
  m_deviceContext->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
}

// Enlaza uno o varios Render Target Views y un Depth Stencil View al Output Merger.
// NumViews dice cuántos RTVs se van a usar.
void
DeviceContext::OMSetRenderTargets(unsigned int NumViews,
  ID3D11RenderTargetView* const* ppRenderTargetViews,
  ID3D11DepthStencilView* pDepthStencilView) {
  // Validación básica de parámetros.
  if (!ppRenderTargetViews && !pDepthStencilView) {
    ERROR("DeviceContext", "OMSetRenderTargets",
      "Both ppRenderTargetViews and pDepthStencilView are nullptr");
    return;
  }

  if (NumViews > 0 && !ppRenderTargetViews) {
    ERROR("DeviceContext", "OMSetRenderTargets",
      "ppRenderTargetViews is nullptr, but NumViews > 0");
    return;
  }

  m_deviceContext->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilView);
}

// Define la topología de las primitivas que se van a dibujar.
// Por ejemplo triángulos, líneas, etc.
void
DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) {
  if (Topology == D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED) {
    ERROR("DeviceContext", "IASetPrimitiveTopology",
      "Topology is D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED");
    return;
  }

  m_deviceContext->IASetPrimitiveTopology(Topology);
}

// Limpia un Render Target View con un color sólido.
// ColorRGBA es un arreglo con RGBA en flotantes.
void
DeviceContext::ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView,
  const float ColorRGBA[4]) {
  if (!pRenderTargetView) {
    ERROR("DeviceContext", "ClearRenderTargetView", "pRenderTargetView is nullptr");
    return;
  }
  if (!ColorRGBA) {
    ERROR("DeviceContext", "ClearRenderTargetView", "ColorRGBA is nullptr");
    return;
  }

  m_deviceContext->ClearRenderTargetView(pRenderTargetView, ColorRGBA);
}

// Limpia la vista de profundidad y/o esténcil.
// ClearFlags indica qué se limpia (depth, stencil o ambos).
void
DeviceContext::ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView,
  unsigned int ClearFlags,
  float Depth,
  UINT8 Stencil) {
  if (!pDepthStencilView) {
    ERROR("DeviceContext", "ClearDepthStencilView",
      "pDepthStencilView is nullptr");
    return;
  }

  if ((ClearFlags & (D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL)) == 0) {
    ERROR("DeviceContext", "ClearDepthStencilView",
      "Invalid ClearFlags: must include D3D11_CLEAR_DEPTH or D3D11_CLEAR_STENCIL");
    return;
  }

  m_deviceContext->ClearDepthStencilView(pDepthStencilView, ClearFlags, Depth, Stencil);
}

// Enlaza constant buffers al Vertex Shader.
// StartSlot es el primer slot de buffer y NumBuffers cuántos buffers se envían.
void
DeviceContext::VSSetConstantBuffers(unsigned int StartSlot,
  unsigned int NumBuffers,
  ID3D11Buffer* const* ppConstantBuffers) {
  if (!ppConstantBuffers) {
    ERROR("DeviceContext", "VSSetConstantBuffers", "ppConstantBuffers is nullptr");
    return;
  }

  m_deviceContext->VSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

// Enlaza constant buffers al Pixel Shader.
void
DeviceContext::PSSetConstantBuffers(unsigned int StartSlot,
  unsigned int NumBuffers,
  ID3D11Buffer* const* ppConstantBuffers) {
  if (!ppConstantBuffers) {
    ERROR("DeviceContext", "PSSetConstantBuffers", "ppConstantBuffers is nullptr");
    return;
  }

  m_deviceContext->PSSetConstantBuffers(StartSlot, NumBuffers, ppConstantBuffers);
}

// Llama a DrawIndexed para dibujar usando el index buffer actual.
// IndexCount es la cantidad de índices que se van a dibujar.
void
DeviceContext::DrawIndexed(unsigned int IndexCount,
  unsigned int StartIndexLocation,
  int BaseVertexLocation) {
  if (IndexCount == 0) {
    ERROR("DeviceContext", "DrawIndexed", "IndexCount is zero");
    return;
  }

  m_deviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}
