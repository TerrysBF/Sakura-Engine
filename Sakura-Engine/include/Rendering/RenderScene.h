/**
 * @file RenderScene.h
 * @brief Declara la clase RenderScene usada durante el render de un frame.
 */
#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class Skybox;

/**
 * @class RenderScene
 * @brief Representa los elementos visibles que se van a renderizar en un frame.
 *
 * Esta clase sirve como una estructura temporal que organiza los objetos
 * antes de enviarlos al renderer. Separa los objetos por tipo y guarda
 * información como luces y skybox.
 */
class
  RenderScene {
public:

  /**
   * @brief Limpia todos los datos de la escena.
   *
   * Se utiliza al inicio de cada frame para eliminar la información
   * anterior y preparar nuevos datos de render.
   */
  void clear();

public:

  /**
   * @brief Lista de objetos opacos.
   *
   * Estos objetos se renderizan normalmente primero.
   */
  std::vector<RenderObject> opaqueObjects;

  /**
   * @brief Lista de objetos transparentes.
   *
   * Estos objetos suelen ordenarse por distancia antes de renderizarse.
   */
  std::vector<RenderObject> transparentObjects;

  /**
   * @brief Lista de luces direccionales activas.
   */
  std::vector<LightData> directionalLights;

  /**
   * @brief Skybox activo en el frame actual.
   */
  Skybox* skybox = nullptr;
};