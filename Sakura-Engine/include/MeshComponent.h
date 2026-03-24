#pragma once
#include "Prerequisites.h"
#include "ECS\Component.h"

class DeviceContext;

/**
 * @class MeshComponent
 * @brief Guarda la "forma" física (geometría) de un objeto en el juego.
 * * Es un componente que le das a un Actor para que tenga una apariencia 3D.
 * Contiene la lista de puntos (vértices) y el orden en que se conectan (índices).
 */
class
  MeshComponent : public Component {
public:
  /**
   * @brief Constructor: Crea una malla vacía y le asigna el tipo MESH.
   */
  MeshComponent() : m_numVertex(0), m_numIndex(0), Component(ComponentType::MESH) {}

  /**
   * @brief Destructor: Se asegura de limpiar el componente al destruirlo.
   */
  virtual
    ~MeshComponent() = default;

  /**
   * @brief Configuración inicial de la malla.
   */
  void
    init() override {};

  /**
   * @brief Actualiza la malla (por si los puntos se mueven o cambian).
   * @param deltaTime Tiempo entre frames.
   */
  void
    update(float deltaTime) override {};

  /**
   * @brief Envía la geometría a la tarjeta de video para que sea dibujada.
   * @param deviceContext El encargado de dar las órdenes de dibujo.
   */
  void
    render(DeviceContext& deviceContext) override {};

  /**
   * @brief Borra los datos de la malla para liberar memoria.
   */
  void
    destroy() override {};

public:
  /** @brief Nombre descriptivo de esta parte del modelo (ej: "Rueda_Delantera"). */
  std::string m_name;

  /** @brief Lista de todos los puntos (punteros) que forman la figura. */
  std::vector<SimpleVertex> m_vertex;

  /** @brief Lista de números que dicen cómo conectar los vértices para formar triángulos. */
  std::vector<unsigned int> m_index;

  /** @brief Cantidad total de puntos que tiene esta malla. */
  int m_numVertex;

  /** @brief Cantidad total de conexiones (índices) que tiene esta malla. */
  int m_numIndex;
};