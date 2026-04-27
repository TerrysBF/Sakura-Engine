/**
 * @file RenderTypes.h
 * @brief Define tipos, estructuras y enums usados en el sistema de render.
 */
#pragma once
#include "Prerequisites.h"

class Mesh;
class MaterialInstance;

/**
 * @enum MaterialDomain
 * @brief Define el tipo de material según cómo se renderiza.
 */
enum class
  MaterialDomain {
  Opaque = 0,
  Masked,
  Transparent
};

/**
 * @enum BlendMode
 * @brief Define el tipo de mezcla de colores en el render.
 */
enum class
  BlendMode {
  Opaque = 0,
  Alpha,
  Additive,
  PremultipliedAlpha
};

/**
 * @enum RenderPassType
 * @brief Define los diferentes passes de render.
 */
enum class
  RenderPassType {
  Shadow = 0,
  Opaque,
  Skybox,
  Transparent,
  Editor
};

/**
 * @enum LightType
 * @brief Define los tipos de luz disponibles.
 */
enum class
  LightType {
  Directional = 0,
  Point,
  Spot
};

/**
 * @struct LightData
 * @brief Contiene la información de una luz en la escena.
 *
 * Incluye datos como tipo, color, dirección y posición.
 */
struct
  LightData {

  /**
   * @brief Tipo de luz.
   */
  LightType type = LightType::Directional;

  /**
   * @brief Color de la luz.
   */
  EU::Vector3 color = EU::Vector3(1.0f, 1.0f, 1.0f);

  /**
   * @brief Intensidad de la luz.
   */
  float intensity = 1.0f;

  /**
   * @brief Dirección de la luz.
   */
  EU::Vector3 direction = EU::Vector3(0.0f, -1.0f, 0.0f);

  /**
   * @brief Alcance de la luz.
   */
  float range = 0.0f;

  /**
   * @brief Posición de la luz.
   */
  EU::Vector3 position = EU::Vector3(0.0f, 0.0f, 0.0f);

  /**
   * @brief Ángulo del cono en luces tipo spot.
   */
  float spotAngle = 0.0f;
};

/**
 * @struct MaterialParams
 * @brief Parámetros básicos de un material.
 *
 * Se utilizan para controlar propiedades visuales como color,
 * metalicidad y rugosidad.
 */
struct
  MaterialParams {

  /**
   * @brief Color base del material.
   */
  XMFLOAT4 baseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

  /**
   * @brief Valor metálico.
   */
  float metallic = 1.0f;

  /**
   * @brief Rugosidad del material.
   */
  float roughness = 1.0f;

  /**
   * @brief Ambient occlusion.
   */
  float ao = 1.0f;

  /**
   * @brief Intensidad del mapa de normales.
   */
  float normalScale = 1.0f;

  /**
   * @brief Intensidad de emisión.
   */
  float emissiveStrength = 1.0f;

  /**
   * @brief Umbral de corte para transparencias.
   */
  float alphaCutoff = 0.5f;
};

/**
 * @struct CBPerFrame
 * @brief Constant buffer que se actualiza cada frame.
 *
 * Contiene matrices de cámara y datos de iluminación.
 */
struct
  CBPerFrame {

  /**
   * @brief Matriz de vista.
   */
  XMFLOAT4X4 View{};

  /**
   * @brief Matriz de proyección.
   */
  XMFLOAT4X4 Projection{};

  /**
   * @brief Matriz de luz para sombras.
   */
  XMFLOAT4X4 LightViewProjection{};

  /**
   * @brief Posición de la cámara.
   */
  EU::Vector3 CameraPos{};

  float pad0 = 0.0f;

  /**
   * @brief Dirección de la luz principal.
   */
  EU::Vector3 LightDir = EU::Vector3(0.0f, -1.0f, 0.0f);

  float pad1 = 0.0f;

  /**
   * @brief Color de la luz principal.
   */
  EU::Vector3 LightColor = EU::Vector3(1.0f, 1.0f, 1.0f);

  float pad2 = 0.0f;
};

/**
 * @struct CBPerObject
 * @brief Constant buffer por objeto.
 *
 * Contiene la transformación del objeto.
 */
struct
  CBPerObject {

  /**
   * @brief Matriz de transformación mundial.
   */
  XMFLOAT4X4 World{};
};

/**
 * @struct CBPerMaterial
 * @brief Constant buffer por material.
 *
 * Contiene los parámetros que usa el shader.
 */
struct
  CBPerMaterial {

  /**
   * @brief Color base.
   */
  XMFLOAT4 BaseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

  /**
   * @brief Valor metálico.
   */
  float Metallic = 1.0f;

  /**
   * @brief Rugosidad.
   */
  float Roughness = 1.0f;

  /**
   * @brief Ambient occlusion.
   */
  float AO = 1.0f;

  /**
   * @brief Intensidad de normales.
   */
  float NormalScale = 1.0f;

  /**
   * @brief Intensidad emisiva.
   */
  float EmissiveStrength = 1.0f;

  /**
   * @brief Umbral de alpha.
   */
  float AlphaCutoff = 0.0f;

  float pad0 = 0.0f;
  float pad1 = 0.0f;
  float pad2 = 0.0f;
  float pad3 = 0.0f;
  float pad4 = 0.0f;
  float pad5 = 0.0f;
};

/**
 * @struct RenderObject
 * @brief Representa un objeto listo para renderizar.
 *
 * Contiene la malla, material y datos necesarios
 * para dibujarlo en pantalla.
 */
struct
  RenderObject {

  /**
   * @brief Malla del objeto.
   */
  Mesh* mesh = nullptr;

  /**
   * @brief Instancia principal del material.
   */
  MaterialInstance* materialInstance = nullptr;

  /**
   * @brief Lista de materiales por submalla.
   */
  std::vector<MaterialInstance*> materialInstances;

  /**
   * @brief Transformación del objeto.
   */
  XMMATRIX world = XMMatrixIdentity();

  /**
   * @brief Indica si proyecta sombras.
   */
  bool castShadow = true;

  /**
   * @brief Indica si es transparente.
   */
  bool transparent = false;

  /**
   * @brief Distancia a la cámara.
   */
  float distanceToCamera = 0.0f;
};