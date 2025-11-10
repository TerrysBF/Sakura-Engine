#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;

// Clase que envuelve una textura 2D y su SRV en D3D11.
// Puede venir de archivo o crearse en memoria (para render target, depth, etc.).
class Texture {
public:
  // Constructor por defecto.
  Texture() = default;

  // Destructor por defecto. La textura real se libera con destroy().
  ~Texture() = default;

  // Carga una textura desde archivo y crea su Shader Resource View.
  // - device: device de D3D11.
  // - textureName: nombre base del archivo (sin extensión o como la uses).
  // - extensionType: tipo de archivo (PNG, JPG, DDS).
  // Devuelve S_OK si todo salió bien.
  HRESULT init(Device& device,
    const std::string& textureName,
    ExtensionType extensionType);

  // Crea una textura 2D vacía en memoria (por ejemplo para depth o render target).
  // - width / height: tamaño en píxeles.
  // - Format: formato DXGI.
  // - BindFlags: para qué se va a usar (SHADER_RESOURCE, RENDER_TARGET, etc.).
  // - sampleCount / qualityLevels: configuración de MSAA.
  HRESULT init(Device& device,
    unsigned int width,
    unsigned int height,
    DXGI_FORMAT Format,
    unsigned int BindFlags,
    unsigned int sampleCount = 1,
    unsigned int qualityLevels = 0);

  // Crea una SRV a partir de otra textura ya existente, cambiando el formato de la vista.
  // Muy útil cuando quieres exponer una textura creada antes al shader.
  HRESULT init(Device& device, Texture& textureRef, DXGI_FORMAT format);

  // Update vacío por ahora (para futuras cosas si se quiere animar/modificar la textura).
  void update();

  // Enlaza la SRV al Pixel Shader.
  // - StartSlot: primer slot donde va la textura (normalmente 0).
  // - NumViews: cuántas SRVs vas a poner (aquí casi siempre 1).
  void render(DeviceContext& deviceContext,
    unsigned int StartSlot,
    unsigned int NumViews);

  // Libera la textura y la SRV de forma segura.
  void destroy();

public:
  // Textura 2D en la GPU.
  ID3D11Texture2D* m_texture = nullptr;

  // Shader Resource View para poder usar la textura en los shaders.
  ID3D11ShaderResourceView* m_textureFromImg = nullptr;

  // Nombre o ruta de la textura (por si se necesita para debug).
  std::string m_textureName;
};
