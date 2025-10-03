#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;

/**
 * @class Texture
 * @brief Envoltura de una textura 2D y su SRV en D3D11.
 *
 * Puede venir de archivo o crearse en memoria (RTV/DSV/UAV).
 * Sirve para cargar, usar en shaders y liberar la textura.
 */
class
  Texture {
public:
  /**
   * @brief Constructor por defecto.
   */
  Texture() = default;

  /**
   * @brief Destructor por defecto.
   * @details No libera solo; usa destroy().
   */
  ~Texture() = default;

  /**
   * @brief Carga una textura desde archivo y crea su SRV.
   * @param device Dispositivo D3D11.
   * @param textureName Ruta/nombre del archivo.
   * @param extensionType Tipo (PNG, JPG, DDS).
   * @return S_OK si ok; HRESULT en error.
   * @post m_texture y m_textureFromImg válidos en éxito.
   */
  HRESULT
    init(Device& device,
      const std::string& textureName,
      ExtensionType extensionType);

  /**
   * @brief Crea una textura 2D vacía en memoria.
   * @param device Dispositivo D3D11.
   * @param width Ancho en píxeles.
   * @param height Alto en píxeles.
   * @param Format Formato DXGI.
   * @param BindFlags Binds (ej. SHADER_RESOURCE, RENDER_TARGET).
   * @param sampleCount MSAA (1 = sin MSAA).
   * @param qualityLevels Calidad de MSAA.
   * @return S_OK si ok; HRESULT en error.
   */
  HRESULT
    init(Device& device,
      unsigned int width,
      unsigned int height,
      DXGI_FORMAT Format,
      unsigned int BindFlags,
      unsigned int sampleCount = 1,
      unsigned int qualityLevels = 0);

  /**
   * @brief Crea una textura tomando otra como referencia (cambia formato).
   * @param device Dispositivo D3D11.
   * @param textureRef Textura base.
   * @param format Nuevo formato DXGI.
   * @return S_OK si ok; HRESULT en error.
   */
  HRESULT
    init(Device& device, Texture& textureRef, DXGI_FORMAT format);

  /**
   * @brief Placeholder para actualizar datos (sin uso por ahora).
   */
  void
    update();

  /**
   * @brief Enlaza la SRV al Pixel Shader.
   * @param deviceContext Contexto D3D11.
   * @param StartSlot Slot inicial.
   * @param NumViews Cuántas vistas (normalmente 1).
   * @pre Debe haberse llamado init() antes.
   */
  void
    render(DeviceContext& deviceContext, unsigned int StartSlot, unsigned int NumViews);

  /**
   * @brief Libera la textura y su SRV.
   * @post m_texture == nullptr y m_textureFromImg == nullptr.
   */
  void
    destroy();

public:
  /**
   * @brief Textura 2D en GPU.
   */
  ID3D11Texture2D* m_texture = nullptr;

  /**
   * @brief Shader Resource View para usar la textura en shaders.
   */
  ID3D11ShaderResourceView* m_textureFromImg = nullptr;

  /**
   * @brief Nombre o ruta del archivo (si aplica).
   */
  std::string m_textureName;
};
