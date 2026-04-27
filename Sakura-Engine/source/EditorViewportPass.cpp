/**
 * @file EditorViewportPass.cpp
 * @brief Implementa la logica de EditorViewportPass dentro del subsistema Utilities.
 * @ingroup utilities
 */
#include "EngineUtilities\Utilities\EditorViewportPass.h"
#include "Device.h"
#include "DeviceContext.h"

HRESULT EditorViewportPass::init(Device& device, unsigned int width, unsigned int height)
{
	return createResources(device, width, height);
}

HRESULT EditorViewportPass::resize(Device& device, unsigned int width, unsigned int height)
{
	if (width < 64) width = 64;
	if (height < 64) height = 64;

	if (width == m_width && height == m_height && isValid())
		return S_OK;

	return createResources(device, width, height);
}

void EditorViewportPass::begin(DeviceContext& deviceContext, const float clearColor[4])
{
	m_rtv.render(deviceContext, m_dsv, 1, clearColor);
}

void EditorViewportPass::clearDepth(DeviceContext& deviceContext)
{
	m_dsv.render(deviceContext);
}

void EditorViewportPass::setViewport(DeviceContext& deviceContext)
{
	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(m_width);
	vp.Height = static_cast<float>(m_height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	deviceContext.m_deviceContext->RSSetViewports(1, &vp);
}

long EditorViewportPass::createResources(Device& device, unsigned int width, unsigned int height)
{
	return S_OK; // stub
}