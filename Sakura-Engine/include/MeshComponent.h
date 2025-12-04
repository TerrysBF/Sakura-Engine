#pragma once
#include "Prerequisites.h"
#include "ECS\Component.h"

class DeviceContext;

/// <summary>
/// Componente ECS que almacena la información de geometría (malla) de un actor.
/// Contiene vértices, índices y contadores básicos de la malla.
/// </summary>
class
  MeshComponent : public Component {
public:
  /// <summary>
  /// Constructor por defecto.
  /// Inicializa la malla con cero vértices e índices y la marca como tipo MESH.
  /// </summary>
  MeshComponent() : m_numVertex(0), m_numIndex(0), Component(ComponentType::MESH) {}

  /// <summary>
  /// Destructor virtual por defecto.
  /// </summary>
  virtual
    ~MeshComponent() = default;

  /// <summary>
  /// Inicializa el componente de malla.
  /// Se puede extender en derivadas para reservar memoria o cargar datos.
  /// </summary>
  void
    init() override {};

  /// <summary>
  /// Actualiza la malla.
  /// Útil para animación de vértices o deformaciones en implementaciones futuras.
  /// </summary>
  /// <param name="deltaTime">Tiempo transcurrido desde la última actualización.</param>
  void
    update(float deltaTime) override {};

  /// <summary>
  /// Renderiza la malla.
  /// Normalmente se usaría para dibujar buffers asociados a la geometría.
  /// </summary>
  /// <param name="deviceContext">Contexto del dispositivo para operaciones gráficas.</param>
  void
    render(DeviceContext& deviceContext) override {};

  /// <summary>
  /// Libera recursos asociados al componente de malla.
  /// </summary>
  void
    destroy() override {};

public:
  // Nombre de la malla.
  std::string m_name;

  // Lista de vértices de la malla.
  std::vector<SimpleVertex> m_vertex;

  // Lista de índices que definen las primitivas de la malla.
  std::vector<unsigned int> m_index;

  // Número total de vértices en la malla.
  int m_numVertex;

  // Número total de índices en la malla.
  int m_numIndex;
};
