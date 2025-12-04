#pragma once
#include "Prerequisites.h"
#include "ECS/Actor.h"     // Actor, getComponent, etc.

#include <vector>

// Forward declarations para no depender de d3d11.h aquí
struct ID3D11Device;
struct ID3D11DeviceContext;

/**
 * @brief Capa de interfaz de usuario basada en Dear ImGui.
 *
 * Muestra:
 * - Barra de menú principal.
 * - Panel de jerarquía (lista de actores de la escena).
 * - Panel de inspector (componentes del actor seleccionado).
 */
class UserInterface
{
public:
  UserInterface() = default;
  ~UserInterface() = default;

  /**
   * @brief Inicializa Dear ImGui con Win32 + DirectX 11.
   * @param hwnd   Handle de la ventana Win32.
   * @param device Dispositivo de D3D11.
   * @param context Contexto inmediato de D3D11.
   */
  void init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* context);

  /**
   * @brief Asocia el vector de actores de la escena para poder mostrarlos
   *        en el panel de jerarquía.
   */
  void setSceneActors(const std::vector<EU::TSharedPointer<Actor>>* actors);

  /**
   * @brief Construye la UI para el frame actual (ventanas ImGui, jerarquía,
   *        inspector, etc.). Debe llamarse una vez por frame antes del render.
   */
  void update();

  /**
   * @brief Renderiza la UI sobre el backbuffer actual.
   * Debe llamarse después de dibujar la escena 3D.
   */
  void render();

  /**
   * @brief Libera los recursos de ImGui. Llamar antes de destruir el device/context.
   */
  void destroy();

private:
  // ---------------------------------------------------------------------
  // Datos de escena
  // ---------------------------------------------------------------------
  const std::vector<EU::TSharedPointer<Actor>>* m_actors = nullptr;
  Actor* m_selectedActor = nullptr;

  // Cache sencillo para editar el Transform del actor seleccionado.
  bool        m_hasCachedTransform = false;
  EU::Vector3 m_cachedPos{ 0.0f, 0.0f, 0.0f };
  EU::Vector3 m_cachedRot{ 0.0f, 0.0f, 0.0f };
  EU::Vector3 m_cachedScale{ 1.0f, 1.0f, 1.0f };

private:
  // Ventanas internas de ImGui
  void drawMainMenuBar_();
  void drawHierarchy_();
  void drawInspector_();
};
