/**
 * @file MaterialInstance.h
 * @brief Declara la clase MaterialInstance dentro del sistema de render.
 */
#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Material;
class DeviceContext;
class Texture;

/**
 * @class MaterialInstance
 * @brief Representa una instancia concreta de un material.
 *
 * Esta clase permite usar un mismo material base, pero con diferentes
 * texturas y parámetros para cada objeto en la escena.
 * Es útil para no duplicar materiales y solo cambiar sus propiedades.
 */
class
  MaterialInstance {
public:

  /**
   * @brief Asigna el material base a la instancia.
   * @param material Puntero al material que se va a usar.
   */
  void setMaterial(Material* material) { m_material = material; }

  /**
   * @brief Asigna la textura de color (albedo).
   * @param texture Textura a utilizar.
   */
  void setAlbedo(Texture* texture) { m_albedo = texture; }

  /**
   * @brief Asigna la textura de normales.
   * @param texture Textura a utilizar.
   */
  void setNormal(Texture* texture) { m_normal = texture; }

  /**
   * @brief Asigna la textura metálica.
   * @param texture Textura a utilizar.
   */
  void setMetallic(Texture* texture) { m_metallic = texture; }

  /**
   * @brief Asigna la textura de rugosidad.
   * @param texture Textura a utilizar.
   */
  void setRoughness(Texture* texture) { m_roughness = texture; }

  /**
   * @brief Asigna la textura de ambient occlusion.
   * @param texture Textura a utilizar.
   */
  void setAO(Texture* texture) { m_ao = texture; }

  /**
   * @brief Asigna la textura emisiva.
   * @param texture Textura a utilizar.
   */
  void setEmissive(Texture* texture) { m_emissive = texture; }

  /**
   * @brief Obtiene el material base.
   * @return Puntero al material actual.
   */
  Material* getMaterial() const { return m_material; }

  /**
   * @brief Obtiene la textura de albedo.
   * @return Puntero a la textura.
   */
  Texture* getAlbedo() const { return m_albedo; }

  /**
   * @brief Obtiene la textura de normales.
   * @return Puntero a la textura.
   */
  Texture* getNormal() const { return m_normal; }

  /**
   * @brief Obtiene la textura metálica.
   * @return Puntero a la textura.
   */
  Texture* getMetallic() const { return m_metallic; }

  /**
   * @brief Obtiene la textura de rugosidad.
   * @return Puntero a la textura.
   */
  Texture* getRoughness() const { return m_roughness; }

  /**
   * @brief Obtiene la textura de ambient occlusion.
   * @return Puntero a la textura.
   */
  Texture* getAO() const { return m_ao; }

  /**
   * @brief Obtiene la textura emisiva.
   * @return Puntero a la textura.
   */
  Texture* getEmissive() const { return m_emissive; }

  /**
   * @brief Obtiene los parámetros del material.
   * @return Referencia a los parámetros modificables.
   */
  MaterialParams& getParams() { return m_params; }

  /**
   * @brief Obtiene los parámetros del material en modo constante.
   * @return Referencia constante a los parámetros.
   */
  const MaterialParams& getParams() const { return m_params; }

  /**
   * @brief Enlaza las texturas en el pipeline gráfico.
   *
   * Este método envía las texturas al shader para que puedan ser utilizadas
   * durante el render.
   *
   * @param deviceContext Contexto del dispositivo gráfico.
   */
  void bindTextures(DeviceContext& deviceContext) const;

private:

  /**
   * @brief Material base asociado a la instancia.
   */
  Material* m_material = nullptr;

  /**
   * @brief Textura de color principal.
   */
  Texture* m_albedo = nullptr;

  /**
   * @brief Textura de normales.
   */
  Texture* m_normal = nullptr;

  /**
   * @brief Textura metálica.
   */
  Texture* m_metallic = nullptr;

  /**
   * @brief Textura de rugosidad.
   */
  Texture* m_roughness = nullptr;

  /**
   * @brief Textura de ambient occlusion.
   */
  Texture* m_ao = nullptr;

  /**
   * @brief Textura emisiva.
   */
  Texture* m_emissive = nullptr;

  /**
   * @brief Parámetros del material.
   */
  MaterialParams m_params;
};