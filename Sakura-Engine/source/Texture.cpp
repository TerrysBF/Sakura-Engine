/**
 * @file Texture.cpp
 * @brief Implementación básica de Texture: crear, bind y liberar.
 */

#include "Texture.h"
#include "Device.h"
#include "DeviceContext.h"

 /**
  * Inicializa una textura cargándola desde un archivo de imagen (DDS/PNG/JPG)
  * y crea la Shader Resource View para poder usarla en los shaders.
  */
HRESULT
Texture::init(Device& device,
  const std::string& textureName,
  ExtensionType extensionType) {
  // Revisar que el device exista
  if (!device.m_device) {
    ERROR("Texture", "init", "Device is null.");
    return E_POINTER;
  }
  // Revisar que el nombre base de la textura no esté vacío
  if (textureName.empty()) {
    ERROR("Texture", "init", "Texture name cannot be empty.");
    return E_INVALIDARG;
  }

  HRESULT hr = S_OK;

  // Elegir el nombre de archivo según el tipo de extensión que se pasa
  switch (extensionType) {
  case DDS:
    // Agregar extensión .dds al nombre base
    m_textureName = textureName + ".dds";
    break;
  case PNG:
    // Agregar extensión .png al nombre base
    m_textureName = textureName + ".png";
    break;
  case JPG:
    // Agregar extensión .jpg al nombre base
    m_textureName = textureName + ".jpg";
    break;
  default:
    // Si no es ninguno de los soportados, se marca error
    ERROR("Texture", "init", "Unsupported extension type");
    return E_INVALIDARG;
  }

  // Cargar la textura desde archivo y crear la SRV en un solo paso
  hr = D3DX11CreateShaderResourceViewFromFile(
    device.m_device,        // device de Direct3D
    m_textureName.c_str(),  // ruta al archivo de textura
    nullptr,                // sin parámetros extra
    nullptr,                // sin thread pool
    &m_textureFromImg,      // aquí sale la SRV
    nullptr                 // sin blob de errores
  );

  // Si la carga falla, avisamos y regresamos el error
  if (FAILED(hr)) {
    ERROR("Texture", "init",
      ("Failed to load texture. Verify filepath: " + m_textureName).c_str());
    return hr;
  }

  return S_OK;
}

/**
 * Crea una textura 2D vacía en GPU.
 * Esta se usa normalmente para depth, render targets, etc.
 */
HRESULT
Texture::init(Device& device,
  unsigned int width,
  unsigned int height,
  DXGI_FORMAT Format,
  unsigned int BindFlags,
  unsigned int sampleCount,
  unsigned int qualityLevels) {
  // Revisar que el device exista
  if (!device.m_device) {
    ERROR("Texture", "init", "Device is null.");
    return E_POINTER;
  }
  // Comprobar que las dimensiones sean válidas
  if (width == 0 || height == 0) {
    ERROR("Texture", "init", "Width and height must be greater than 0");
    return E_INVALIDARG;
  }

  // Descripción básica de una textura 2D
  D3D11_TEXTURE2D_DESC desc;
  memset(&desc, 0, sizeof(desc));
  desc.Width = width;                // ancho en pixeles
  desc.Height = height;              // alto en pixeles
  desc.MipLevels = 1;                // solo un mip
  desc.ArraySize = 1;                // una sola textura en el array
  desc.Format = Format;              // formato de la textura (color o depth)
  desc.SampleDesc.Count = sampleCount;      // muestras para MSAA
  desc.SampleDesc.Quality = qualityLevels;  // nivel de calidad de MSAA
  desc.Usage = D3D11_USAGE_DEFAULT;         // uso por defecto
  desc.BindFlags = BindFlags;               // para qué se va a usar (RTV, DSV, SRV, etc.)
  desc.CPUAccessFlags = 0;                  // CPU no la toca directamente
  desc.MiscFlags = 0;

  // Crear la textura en GPU
  HRESULT hr = device.CreateTexture2D(&desc, nullptr, &m_texture);

  if (FAILED(hr)) {
    ERROR("Texture", "init",
      ("Failed to create texture with specified params. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  return S_OK;
}

/**
 * Crea una Shader Resource View (SRV) a partir de otra textura ya existente.
 * Sirve para exponer esa textura a los shaders con el formato que se pide.
 */
HRESULT
Texture::init(Device& device, Texture& textureRef, DXGI_FORMAT format) {
  // Revisar que el device exista
  if (!device.m_device) {
    ERROR("Texture", "init", "Device is null.");
    return E_POINTER;
  }
  // Revisar que la textura base tenga algo
  if (!textureRef.m_texture) {
    ERROR("Texture", "init", "Texture is null.");
    return E_POINTER;
  }

  // Descripción simple de la SRV para una textura 2D
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = format;                        // formato que queremos ver
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1;               // un solo mip
  srvDesc.Texture2D.MostDetailedMip = 0;         // mip 0 es el más detallado

  // Crear la SRV usando la textura original
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
 * Método vacío por ahora, se podría usar si se quiere actualizar algo de la textura.
 */
void
Texture::update() {
}

/**
 * Enlaza la SRV de la textura al Pixel Shader en el slot indicado.
 * Así el shader puede samplear la textura.
 */
void
Texture::render(DeviceContext& deviceContext,
  unsigned int StartSlot,
  unsigned int NumViews) {
  // Revisar que el DeviceContext exista
  if (!deviceContext.m_deviceContext) {
    ERROR("Texture", "render", "Device Context is null.");
    return;
  }

  // Solo se hace el bind si la SRV es válida
  if (m_textureFromImg) {
    deviceContext.PSSetShaderResources(StartSlot, NumViews, &m_textureFromImg);
  }
}

/**
 * Libera los recursos de la textura y de su SRV.
 * Se llama cuando ya no se va a usar para evitar leaks.
 */
void
Texture::destroy() {
  SAFE_RELEASE(m_texture);
  SAFE_RELEASE(m_textureFromImg);
}
