#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;

/**
 * @class Texture
 * @brief Gestiona imágenes y datos 2D en la memoria de la tarjeta de video.
 * * Las texturas son los "trajes" de los modelos 3D (colores) o contenedores de datos especiales.
 * Esta clase maneja tanto la textura física como el SRV (Shader Resource View), que es el "permiso"
 * para que un Shader pueda leer esa imagen.
 */
class Texture {
public:
  /**
   * @brief Constructor: Crea un objeto de textura vacío.
   */
  Texture() = default;

  /**
   * @brief Destructor: No libera los recursos automáticamente, usa destroy().
   */
  ~Texture() = default;

  /**
   * @brief Carga una imagen real (como un .png o .jpg) desde tu carpeta de activos.
   * @param device El creador de recursos de la GPU.
   * @param textureName El nombre del archivo (ej: "Muro_Piedra").
   * @param extensionType El formato del archivo (PNG, JPG, DDS).
   * @return S_OK si la imagen se encontró y se subió con éxito a la GPU.
   */
  HRESULT init(Device& device,
    const std::string& textureName,
    ExtensionType extensionType);

  /**
   * @brief Crea una textura vacía "en blanco" directamente en la memoria de la GPU.
   * * Muy útil para crear Buffers de profundidad o superficies donde el motor dibujará luego.
   * @param width Ancho en píxeles.
   * @param height Alto en píxeles.
   * @param Format El tipo de datos (ej: R8G8B8A8 para color normal).
   * @param BindFlags Para qué se usará (como textura de dibujo, de profundidad, etc).
   * @param sampleCount Para suavizado de bordes (Antialiasing), por defecto es 1.
   * @param qualityLevels Calidad del suavizado, por defecto es 0.
   */
  HRESULT init(Device& device,
    unsigned int width,
    unsigned int height,
    DXGI_FORMAT Format,
    unsigned int BindFlags,
    unsigned int sampleCount = 1,
    unsigned int qualityLevels = 0);

  /**
   * @brief Crea un acceso (SRV) a una textura que ya existe.
   * * Es como crear un "acceso directo" a un archivo para que el Shader lo pueda ver.
   */
  HRESULT init(Device& device, Texture& textureRef, DXGI_FORMAT format);

  /**
   * @brief Espacio para lógica de actualización (por ejemplo, si la textura fuera un video).
   */
  void update();

  /**
   * @brief Conecta la textura al Pixel Shader para que aparezca en los objetos.
   * @param deviceContext El encargado de enviar la textura a la GPU.
   * @param StartSlot El "canal" donde se conectará (normalmente el 0).
   * @param NumViews Cuántas texturas estamos enviando a la vez (normalmente 1).
   */
  void render(DeviceContext& deviceContext,
    unsigned int StartSlot,
    unsigned int NumViews);

  /**
   * @brief Limpia y borra la textura de la memoria de video.
   * ¡Fundamental para evitar que la PC se quede sin memoria de video!
   */
  void destroy();

public:
  /** @brief El recurso binario de la textura en la GPU. */
  ID3D11Texture2D* m_texture = nullptr;

  /** @brief La "Vista" (Shader Resource View) que permite al Shader leer la textura. */
  ID3D11ShaderResourceView* m_textureFromImg = nullptr;

  /** @brief Nombre identificador para saber qué textura es al hacer pruebas (debug). */
  std::string m_textureName;
};