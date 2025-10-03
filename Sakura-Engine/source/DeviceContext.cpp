/**
 * @file DeviceContext.cpp
 * @brief Implementación simple del contexto: binds, clears y draw.
 */

#include "DeviceContext.h"

 /**
	* @brief Libera el contexto inmediato.
	*/
void
DeviceContext::destroy() {
	SAFE_RELEASE(m_deviceContext);
}

/**
 * @brief Setea los viewports para el rasterizer.
 */
void
DeviceContext::RSSetViewports(unsigned int NumViewports,
	const D3D11_VIEWPORT* pViewports) {
	if (!pViewports) {
		ERROR("DeviceContext", "RSSetViewports", "pViewports is nullptr");
		return;
	}

	m_deviceContext->RSSetViewports(NumViewports, pViewports);
}

/**
 * @brief Enlaza SRVs al Pixel Shader.
 */
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

/**
 * @brief Activa un Input Layout en IA.
 */
void
DeviceContext::IASetInputLayout(ID3D11InputLayout* pInputLayout) {
	if (!pInputLayout) {
		ERROR("DeviceContext", "IASetInputLayout", "pInputLayout is nullptr");
		return;
	}

	m_deviceContext->IASetInputLayout(pInputLayout);
}

/**
 * @brief Asigna el Vertex Shader.
 */
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

/**
 * @brief Asigna el Pixel Shader.
 */
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

/**
 * @brief Copia datos de CPU a un recurso de GPU.
 */
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

/**
 * @brief Enlaza vertex buffers al Input Assembler.
 */
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

/**
 * @brief Enlaza el index buffer al Input Assembler.
 */
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

/**
 * @brief Enlaza samplers al Pixel Shader.
 */
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

/**
 * @brief Setea el estado de rasterización.
 */
void
DeviceContext::RSSetState(ID3D11RasterizerState* pRasterizerState) {
	if (!pRasterizerState) {
		ERROR("DeviceContext", "RSSetState", "pRasterizerState is nullptr");
		return;
	}
	m_deviceContext->RSSetState(pRasterizerState);
}

/**
 * @brief Setea el estado de blending en OM.
 */
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

/**
 * @brief Enlaza RTVs y DSV en el Output Merger.
 */
void
DeviceContext::OMSetRenderTargets(unsigned int NumViews,
	ID3D11RenderTargetView* const* ppRenderTargetViews,
	ID3D11DepthStencilView* pDepthStencilView) {
	// Validación básica
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

/**
 * @brief Define la topología (triángulos, líneas, etc.).
 */
void
DeviceContext::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) {
	if (Topology == D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED) {
		ERROR("DeviceContext", "IASetPrimitiveTopology",
			"Topology is D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED");
		return;
	}

	m_deviceContext->IASetPrimitiveTopology(Topology);
}

/**
 * @brief Limpia un RTV con un color.
 */
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

/**
 * @brief Limpia el DSV (depth/stencil).
 * @note Debe incluir D3D11_CLEAR_DEPTH y/o D3D11_CLEAR_STENCIL.
 */
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

/**
 * @brief Enlaza constant buffers al Vertex Shader.
 */
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

/**
 * @brief Enlaza constant buffers al Pixel Shader.
 */
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

/**
 * @brief Dibuja usando un index buffer.
 */
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
