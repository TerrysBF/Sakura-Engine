/**
 * @file LightComponent.h
 * @brief Declara el componente de luz dentro del sistema ECS.
 */
#pragma once
#include "Prerequisites.h"
#include "ECS/Component.h"
#include "Rendering/RenderTypes.h"

class DeviceContext;

/**
 * @class LightComponent
 * @brief Representa una luz dentro de una entidad.
 *
 * Este componente almacena la información necesaria para
 * representar una luz en la escena, además de indicar si
 * puede generar sombras.
 */
class
  LightComponent : public Component {
public:

  /**
   * @brief Constructor por defecto.
   *
   * Inicializa el componente como un componente de tipo genérico.
   */
  LightComponent()
    : Component(ComponentType::NONE) {}

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
   * @brief Obtiene los datos de la luz.
   * @return Referencia a la estructura de datos de la luz.
   */
  LightData& getLightData() { return m_light; }

  /**
   * @brief Obtiene los datos de la luz en modo lectura.
   * @return Referencia constante a la estructura de datos de la luz.
   */
  const LightData& getLightData() const { return m_light; }

  /**
   * @brief Define si la luz genera sombras.
   * @param value Valor que indica si las sombras están habilitadas.
   */
  void setCastShadow(bool value) { m_castShadow = value; }

  /**
   * @brief Indica si la luz genera sombras.
   * @return true si genera sombras.
   * @return false si no genera sombras.
   */
  bool canCastShadow() const { return m_castShadow; }

private:

  /**
   * @brief Información de la luz.
   */
  LightData m_light;

  /**
   * @brief Indica si la luz proyecta sombras.
   */
  bool m_castShadow = false;
};