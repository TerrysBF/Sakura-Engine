#pragma once
#include "Prerequisites.h"
#include "Component.h"
#include <vector>
#include <type_traits>

class DeviceContext;

/**
 * @class Entity
 * @brief Entidad base del ECS: contiene y administra componentes.
 */
class
  Entity {
public:
  Entity() : m_isActive(true), m_id(0) {}
  virtual ~Entity() = default;

  /// Inicializar entidad
  virtual void init() = 0;

  /// Actualizar lógica de la entidad
  virtual void update(float deltaTime, DeviceContext& deviceContext) = 0;

  /// Renderizar entidad
  virtual void render(DeviceContext& deviceContext) = 0;

  /// Liberar recursos de la entidad
  virtual void destroy() = 0;

  /**
   * @brief Agrega un componente a la entidad (puntero crudo).
   * @tparam T Tipo del componente (debe heredar de Component).
   * @param component Puntero al componente ya creado con `new`.
   */
  template<typename T>
  T* addComponent(T* component) {
    static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
    m_components.push_back(component);
    return component;
  }

  /**
   * @brief Obtiene el primer componente de tipo T.
   * @tparam T Tipo de componente a buscar.
   * @return Puntero a T si existe, nullptr si no.
   */
  template<typename T>
  T* getComponent() {
    static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
    for (Component* c : m_components) {
      if (auto* asT = dynamic_cast<T*>(c)) {
        return asT;
      }
    }
    return nullptr;
  }

protected:
  bool m_isActive;
  int  m_id;
  std::vector<Component*> m_components;  ///< No-owning / owning según destroy()
};
