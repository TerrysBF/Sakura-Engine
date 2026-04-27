/**
 * @file Mesh.h
 * @brief Declara las estructuras para manejar mallas dentro del sistema de render.
 */
#pragma once
#include "Prerequisites.h"
#include "Buffer.h"

 /**
  * @struct Submesh
  * @brief Representa una parte de una malla que se puede renderizar.
  *
  * Una submalla contiene sus propios buffers de vértices e índices,
  * además de información necesaria para dibujarla correctamente.
  */
struct
  Submesh {

  /**
   * @brief Buffer de vértices de la submalla.
   */
  Buffer vertexBuffer;

  /**
   * @brief Buffer de índices de la submalla.
   */
  Buffer indexBuffer;

  /**
   * @brief Número total de índices a dibujar.
   */
  unsigned int indexCount = 0;

  /**
   * @brief Posición inicial dentro del index buffer.
   */
  unsigned int startIndex = 0;

  /**
   * @brief Índice del material que se usará para esta submalla.
   */
  unsigned int materialSlot = 0;
};

/**
 * @class Mesh
 * @brief Representa una malla compuesta por varias submallas.
 *
 * Esta clase agrupa diferentes submallas para poder renderizar
 * objetos más complejos que usan distintos materiales o partes.
 */
class
  Mesh {
public:

  /**
   * @brief Obtiene las submallas de forma modificable.
   * @return Referencia al vector de submallas.
   */
  std::vector<Submesh>& getSubmeshes() { return m_submeshes; }

  /**
   * @brief Obtiene las submallas en modo solo lectura.
   * @return Referencia constante al vector de submallas.
   */
  const std::vector<Submesh>& getSubmeshes() const { return m_submeshes; }

  /**
   * @brief Libera los recursos de todas las submallas.
   *
   * Se encarga de destruir los buffers de vértices e índices
   * y limpiar la lista de submallas.
   */
  void
    destroy() {
    for (Submesh& submesh : m_submeshes) {
      submesh.vertexBuffer.destroy();
      submesh.indexBuffer.destroy();
    }
    m_submeshes.clear();
  }

private:

  /**
   * @brief Lista de submallas que componen la malla.
   */
  std::vector<Submesh> m_submeshes;
};