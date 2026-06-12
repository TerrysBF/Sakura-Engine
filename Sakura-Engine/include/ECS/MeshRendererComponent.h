/**
 * @file MeshRendererComponent.h
 * @brief Declara el componente encargado de renderizar mallas.
 */
#pragma once
#include "Prerequisites.h"
#include "ECS/Component.h"

class Mesh;
class MaterialInstance;
class DeviceContext;

/**
 * @class MeshRendererComponent
 * @brief Componente encargado de asociar una malla y materiales a una entidad.
 *
 * Este componente almacena la geometría que será renderizada,
 * junto con los materiales necesarios para dibujar el objeto
 * dentro de la escena.
 */
class
  MeshRendererComponent : public Component {
public:

  /**
   * @brief Constructor por defecto.
   */
  MeshRendererComponent()
    : Component(ComponentType::MESH) {}

  /**
   * @brief Inicializa el componente.
   */
  void init() override {}

  /**
   * @brief Actualiza el componente.
   * @param deltaTime Tiempo transcurrido desde el último frame.
   */
  void update(float deltaTime) override {}

  /**
   * @brief Renderiza el componente.
   * @param deviceContext Contexto gráfico utilizado para renderizar.
   */
  void render(DeviceContext& deviceContext) override {}

  /**
   * @brief Libera los recursos asociados al componente.
   */
  void destroy() override {}

  /**
   * @brief Asigna la malla que utilizará el componente.
   * @param mesh Malla a renderizar.
   */
  void setMesh(Mesh* mesh) { m_mesh = mesh; }

  /**
   * @brief Obtiene la malla asociada.
   * @return Puntero a la malla.
   */
  Mesh* getMesh() const { return m_mesh; }

  /**
   * @brief Asigna un material principal.
   * @param materialInstance Material que será utilizado.
   */
  void setMaterialInstance(MaterialInstance* materialInstance) {
    m_materialInstance = materialInstance;
    m_materialInstances.clear();

    if (materialInstance) {
      m_materialInstances.push_back(materialInstance);
    }
  }

  /**
   * @brief Obtiene el material principal.
   * @return Puntero al material principal.
   */
  MaterialInstance* getMaterialInstance() const { return m_materialInstance; }

  /**
   * @brief Asigna varios materiales al componente.
   * @param materialInstances Lista de materiales.
   */
  void setMaterialInstances(const std::vector<MaterialInstance*>& materialInstances) {
    m_materialInstances = materialInstances;
    m_materialInstance = m_materialInstances.empty() ? nullptr : m_materialInstances.front();
  }

  /**
   * @brief Agrega un material a la lista de materiales.
   * @param materialInstance Material que se agregará.
   */
  void addMaterialInstance(MaterialInstance* materialInstance) {
    if (!materialInstance) {
      return;
    }

    if (!m_materialInstance) {
      m_materialInstance = materialInstance;
    }

    m_materialInstances.push_back(materialInstance);
  }

  /**
   * @brief Obtiene la lista de materiales.
   * @return Referencia constante a la lista de materiales.
   */
  const std::vector<MaterialInstance*>& getMaterialInstances() const {
    return m_materialInstances;
  }

  /**
   * @brief Indica si el objeto es visible.
   * @return true si es visible.
   * @return false si está oculto.
   */
  bool isVisible() const { return m_visible; }

  /**
   * @brief Define la visibilidad del objeto.
   * @param visible Estado de visibilidad.
   */
  void setVisible(bool visible) { m_visible = visible; }

  /**
   * @brief Indica si el objeto genera sombras.
   * @return true si genera sombras.
   * @return false si no genera sombras.
   */
  bool canCastShadow() const { return m_castShadow; }

  /**
   * @brief Define si el objeto genera sombras.
   * @param value Estado de las sombras.
   */
  void setCastShadow(bool value) { m_castShadow = value; }

private:

  /**
   * @brief Malla asociada al componente.
   */
  Mesh* m_mesh = nullptr;

  /**
   * @brief Material principal del componente.
   */
  MaterialInstance* m_materialInstance = nullptr;

  /**
   * @brief Lista de materiales utilizados por la malla.
   */
  std::vector<MaterialInstance*> m_materialInstances;

  /**
   * @brief Indica si el objeto es visible.
   */
  bool m_visible = true;

  /**
   * @brief Indica si el objeto proyecta sombras.
   */
  bool m_castShadow = true;
};