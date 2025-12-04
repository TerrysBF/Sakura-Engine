#pragma once
#include "Prerequisites.h"
#include "Component.h"

class DeviceContext;

/// <summary>
/// Entidad base del motor.
/// Administra una colección de componentes y define la interfaz común
/// para inicializar, actualizar, renderizar y destruir.
/// </summary>
class
  Entity {
public:
  /// <summary>
  /// Constructor por defecto.
  /// </summary>
  Entity() = default;

  /// <summary>
  /// Destructor virtual.
  /// </summary>
  virtual
    ~Entity() = default;

  /// <summary>
  /// Inicializa la entidad.
  /// Puede usarse en clases derivadas para preparar recursos o estados iniciales.
  /// </summary>
  virtual void
    init() = 0;

  /// <summary>
  /// Actualiza la entidad en cada frame.
  /// </summary>
  /// <param name="deltaTime">Tiempo transcurrido desde la última actualización.</param>
  /// <param name="deviceContext">Contexto del dispositivo para operaciones gráficas.</param>
  virtual void
    update(float deltaTime, DeviceContext& deviceContext) = 0;

  /// <summary>
  /// Renderiza la entidad.
  /// </summary>
  /// <param name="deviceContext">Contexto del dispositivo para operaciones gráficas.</param>
  virtual void
    render(DeviceContext& deviceContext) = 0;

  /// <summary>
  /// Destruye la entidad y libera sus recursos asociados.
  /// </summary>
  virtual void
    destroy() = 0;

  /// <summary>
  /// Agrega un componente a la entidad.
  /// </summary>
  /// <typeparam name="T">Tipo del componente, debe derivar de Component.</typeparam>
  /// <param name="component">Puntero compartido al componente que se va a agregar.</param>
  template <typename T> void
    addComponent(EU::TSharedPointer<T> component) {
    static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");
    m_components.push_back(component.template dynamic_pointer_cast<Component>());
  }

  /// <summary>
  /// Obtiene un componente de la entidad por su tipo.
  /// </summary>
  /// <typeparam name="T">Tipo del componente a obtener.</typeparam>
  /// <returns>Puntero compartido al componente si se encuentra; vacío en caso contrario.</returns>
  template<typename T>
  EU::TSharedPointer<T>
    getComponent() {
    for (auto& component : m_components) {
      EU::TSharedPointer<T> specificComponent = component.template dynamic_pointer_cast<T>();
      if (specificComponent) {
        return specificComponent;
      }
    }
    return EU::TSharedPointer<T>();
  }

private:
protected:
  bool m_isActive;   // Indica si la entidad está activa.
  int m_id;          // Identificador numérico de la entidad.
  std::vector<EU::TSharedPointer<Component>> m_components; // Lista de componentes asociados.
};
