#pragma once
#include "Prerequisites.h"
//#include "ECS\Component.h"
class DeviceContext;

/*
 * Clase MeshComponent
 *
 * Guarda la info de una malla: vértices, índices y cuántos son.
 * La idea es que represente la geometría de un objeto 3D.
 */
class MeshComponent /*: public Component*/ {
public:
  // Constructor por defecto
  // Empieza con contadores de vértices e índices en 0.
  MeshComponent() : m_numVertex(0), m_numIndex(0) {}

  // Destructor virtual por defecto
  virtual ~MeshComponent() = default;

  // Inicializa la malla (por ahora está vacío, pero se puede usar más adelante)
  void init() ;

  // Update de la malla (por ejemplo animaciones de vértices)
  // deltaTime es el tiempo entre frames.
  void update(float deltaTime);

  // Función de render de la malla
  // Aquí normalmente se usaría el DeviceContext para dibujar los buffers.
  void render(DeviceContext& deviceContext) ;

  // Libera o limpia datos de la malla (por ahora vacío)
  void destroy();

public:
  // Nombre de la malla (opcional, solo para identificarla)
  std::string m_name;

  // Lista de vértices de la malla (posición, uv, etc. en SimpleVertex)
  std::vector<SimpleVertex> m_vertex;

  // Lista de índices que dicen cómo se forman los triángulos
  std::vector<unsigned int> m_index;

  // Cantidad total de vértices
  int m_numVertex;

  // Cantidad total de índices
  int m_numIndex;
};
