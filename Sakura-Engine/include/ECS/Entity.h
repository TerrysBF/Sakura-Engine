#pragma once
#include "Prerequisites.h"
#include "Component.h"

#include <vector>
#include <type_traits>

class DeviceContext;

/**
 * @class Entity
 * @brief Base abstracta para cualquier entidad del juego.
 *
 * No se instancia directamente; usaremos derivadas como Actor.
 */
class Entity {
public:
  Entity()
    : m_isActive(true)
    , m_id(-1)
  {}

  virtual ~Entity() = default;

  /// Inicialización de la entidad y sus componentes
  virtual void init() = 0;

  /// Actualización lógica
  virtual void update(float deltaTime, DeviceContext& deviceContext) = 0;

  /// Render de la entidad
  virtual void render(DeviceContext& deviceContext) = 0;

  /// Destrucción / liberación de recursos
  virtual void destroy() = 0;

  /// Agrega un componente (T tiene que heredar de Component)
  template <typename T>
  void addComponent(EU::TSharedPointer<T> component) {
    static_assert(std::is_base_of<Component, T>::value,
      "T must be derived from Component");

    // Conversión TSharedPointer<T> -> TSharedPointer<Component>
    EU::TSharedPointer<Component> basePtr = component;
    m_components.push_back(basePtr);
  }

  /// Busca el primer componente del tipo T
  template <typename T>
  EU::TSharedPointer<T> getComponent() {
    for (auto& component : m_components) {
      // Si tu TSharedPointer tiene dynamic_pointer_cast, esto debería compilar;
      // si no, se puede cambiar por constructor EU::TSharedPointer<T>(component).
      auto specificComponent = component.template dynamic_pointer_cast<T>();
      if (specificComponent) {
        return specificComponent;
      }
    }
    return EU::TSharedPointer<T>(); // nulo
  }

  bool isActive() const { return m_isActive; }
  void setActive(bool active) { m_isActive = active; }

  int  getId() const { return m_id; }
  void setId(int id) { m_id = id; }

protected:
  bool m_isActive;
  int  m_id;
  std::vector<EU::TSharedPointer<Component>> m_components;
};
