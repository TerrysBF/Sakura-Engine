#pragma once
#include "Prerequisites.h"
#include <type_traits>   // por si luego lo usas en templates

class DeviceContext;

/**
 * @class Component
 * @brief Clase base abstracta para todos los componentes del juego.
 */
class Component {
public:
  /// Constructor por defecto: componente “sin tipo”
  Component() : m_type(ComponentType::NONE) {}

  /// Constructor con tipo
  explicit Component(ComponentType type) : m_type(type) {}

  /// Destructor virtual
  virtual ~Component() = default;

  /// Inicializar datos del componente (carga de recursos, etc.)
  virtual void init() = 0;

  /// Lógica por frame
  virtual void update(float deltaTime) = 0;

  /// Dibujado
  virtual void render(DeviceContext& deviceContext) = 0;

  /// Liberar recursos
  virtual void destroy() = 0;

  /// Tipo de componente
  ComponentType getType() const { return m_type; }

protected:
  ComponentType m_type;
};
