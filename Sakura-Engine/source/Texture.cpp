/**
 * @file Texture.cpp
 * @brief Implementación básica de Texture: crear, bind y liberar.
 */

#include "Texture.h"
#include "Device.h"
#include "DeviceContext.h"

 /**
  * @brief Carga una textura desde archivo (DDS/PNG/JPG) y crea su SRV.
  */
HRESULT
Texture::init(Device& device,
  const std::string& textureName,
  ExtensionType extensionType) {
  if (!device.m_device) {
    ERROR("Texture", "init", "Device is null.");
    return E_POINTER;
  }
  if (textureName.empty()) {
    ERROR("Texture", "init", "Texture name cannot be empty.");
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;

  // Elegir nombre de archivo según extensión
  switch (extensionType) {
  case DDS:
    m_textureName = textureName + ".dds";
    break;
  case PNG:
    m_textureName = textureName + ".png";
    break;
  case JPG:
    m_textureName = textureName + ".jpg";
    break;
  default:
    ERROR("Texture", "init", "Unsupported extension type");
    return E_INVALIDARG;
  }

  // Cargar textura desde archivo y crear SRV
  hr = D3DX11CreateShaderResourceViewFromFile(
    device.m_device,
    m_textureName.c_str(),
    nullptr,
    nullptr,
    &m_textureFromImg,
    nullptr
  );

  if (FAILED(hr)) {
    ERROR("Texture", "init",
      ("Failed to load texture. Verify filepath: " + m_textureName).c_str());
    return hr;
  }

  return S_OK;
}

/**
 * @brief Crea una textura 2D vacía en GPU.
 * @param width, height Tamaño en píxeles.
 * @param Format Formato DXGI.
 * @param sampleCount/qualityLevels MSAA (1 = sin MSAA).
 * @return S_OK si sale bien HRESULT si sale error.
 * @note Si width/height son 0, se considera argumento inválido.
 */
HRESULT
Texture::init(Device& device,
  unsigned int width,
  unsigned int height,
  DXGI_FORMAT Format,
  unsigned int BindFlags,
  unsigned int sampleCount,
  unsigned int qualityLevels) {
  if (!device.m_device) {
    ERROR("Texture", "init", "Device is null.");
    return E_POINTER;
  }
  if (width == 0 || height == 0) {
    ERROR("Texture", "init", "Width and height must be greater than 0");
    return E_INVALIDARG;
  }

  // Descripción mínima de la textura
  D3D11_TEXTURE2D_DESC desc;
  memset(&desc, 0, sizeof(desc));
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = Format;
  desc.SampleDesc.Count = sampleCount;
  desc.SampleDesc.Quality = qualityLevels;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = BindFlags;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;

  HRESULT hr = device.CreateTexture2D(&desc, nullptr, &m_texture);

  if (FAILED(hr)) {
    ERROR("Texture", "init",
      ("Failed to create texture with specified params. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  return S_OK;
}

/**
 * @brief Crea una SRV tomando otra textura como base (cambia el formato de la view).
 * @param textureRef Textura ya existente.
 * @param format Formato de la SRV.
 * @return S_OK si sale bien HRESULT si sale error.
 * @note Útil para exponer una textura a los shaders.
 */
HRESULT
Texture::init(Device& device, Texture& textureRef, DXGI_FORMAT format) {
  if (!device.m_device) {
    ERROR("Texture", "init", "Device is null.");
    return E_POINTER;
  }
  if (!textureRef.m_texture) {
    ERROR("Texture", "init", "Texture is null.");
    return E_POINTER;
  }

  // Descripción simple de la SRV (Texture2D, un mip)
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1;
  srvDesc.Texture2D.MostDetailedMip = 0;

  HRESULT hr = device.m_device->CreateShaderResourceView(
    textureRef.m_texture,
    &srvDesc,
    &m_textureFromImg);

  if (FAILED(hr)) {
    ERROR("Texture", "init",
      ("Failed to create shader resource view. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  return S_OK;
}

/**
 * @brief Placeholder de actualización (sin uso por ahora).
 */
void
Texture::update() {
}

/**
 * @brief Enlaza la SRV al Pixel Shader.
 * @param StartSlot Slot inicial.
 * @param NumViews Cuántas vistas (normalmente 1).
 */
void
Texture::render(DeviceContext& deviceContext,
  unsigned int StartSlot,
  unsigned int NumViews) {
  if (!deviceContext.m_deviceContext) {
    ERROR("Texture", "render", "Device Context is null.");
    return;
  }

  if (m_textureFromImg) {
    deviceContext.PSSetShaderResources(StartSlot, NumViews, &m_textureFromImg);
  }
}

/**
 * @brief Libera la textura y/o su SRV.
 */
void
Texture::destroy() {
  SAFE_RELEASE(m_texture);
  SAFE_RELEASE(m_textureFromImg);
}
