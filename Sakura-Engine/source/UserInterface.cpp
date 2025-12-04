#include "UserInterface.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Si se requiere más funcionalidad del motor, sus includes pueden agregarse aquí.

/// <summary>
/// Inicializa ImGui para trabajar con Win32 y DirectX 11.
/// Crea el contexto de ImGui, configura flags básicos y enlaza con la ventana y el device.
/// </summary>
/// <param name="hwnd">Handle de la ventana Win32.</param>
/// <param name="device">Dispositivo de Direct3D 11.</param>
/// <param name="context">Contexto de dispositivo de Direct3D 11.</param>
void UserInterface::init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Navegación con teclado
  // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Docking opcional

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init((HWND)hwnd);
  ImGui_ImplDX11_Init(device, context);
}

/// <summary>
/// Asigna la lista de actores de la escena para que la UI pueda mostrarlos en la jerarquía.
/// También resetea el actor seleccionado y la caché de Transform.
/// </summary>
/// <param name="actors">Puntero a un vector con actores compartidos.</param>
void UserInterface::setSceneActors(const std::vector<EU::TSharedPointer<Actor>>* actors)
{
  m_actors = actors;
  m_selectedActor = nullptr;
  m_hasCachedTransform = false;

  if (m_actors && !m_actors->empty())
  {
    m_selectedActor = (*m_actors)[0].get();
    m_hasCachedTransform = false;
  }
}

/// <summary>
/// Actualiza el frame de la interfaz de usuario.
/// Prepara un nuevo frame de ImGui y dibuja las ventanas principales (menú, jerarquía, inspector).
/// </summary>
void UserInterface::update()
{
  // Comenzar nuevo frame de ImGui
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // Docking opcional (apagado para evitar problemas de versión)
  // ImGuiViewport* vp = ImGui::GetMainViewport();
  // ImGui::DockSpaceOverViewport(vp);

  drawMainMenuBar_();
  drawHierarchy_();
  drawInspector_();
}

/// <summary>
/// Renderiza la interfaz de usuario de ImGui sobre el back buffer actual.
/// </summary>
void UserInterface::render()
{
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

/// <summary>
/// Libera los recursos asociados a ImGui (contexto y backends Win32/DX11).
/// </summary>
void UserInterface::destroy()
{
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

// Ventanas internas

/// <summary>
/// Dibuja la barra de menú principal en la parte superior de la ventana.
/// Incluye menús de ejemplo como File y View.
/// </summary>
void UserInterface::drawMainMenuBar_()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      ImGui::MenuItem("Exit", nullptr, false, false); // Placeholder
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
      ImGui::MenuItem("Hierarchy", nullptr, true, false);
      ImGui::MenuItem("Inspector", nullptr, true, false);
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

/// <summary>
/// Dibuja la ventana "Hierarchy", donde se listan todos los actores de la escena.
/// Permite seleccionar un actor para verlo en el inspector.
/// </summary>
void UserInterface::drawHierarchy_()
{
  ImGui::Begin("Hierarchy");

  if (!m_actors)
  {
    ImGui::Text("No hay actores en la escena.");
    ImGui::End();
    return;
  }

  for (auto& actorPtr : *m_actors)
  {
    Actor* actor = actorPtr.get();
    if (!actor) continue;

    const std::string& nameStr = actor->getName();
    // Label visible: si no tiene nombre, se muestra "Actor"
    const char* visibleLabel = nameStr.empty() ? "Actor" : nameStr.c_str();

    // ID único basado en el puntero del actor (evita conflictos de ID en ImGui)
    ImGui::PushID(actor);

    bool selected = (m_selectedActor == actor);
    if (ImGui::Selectable(visibleLabel, selected))
    {
      m_selectedActor = actor;
      m_hasCachedTransform = false; // reset cache al cambiar de actor
    }

    ImGui::PopID();
  }

  ImGui::End();
}

/// <summary>
/// Dibuja la ventana "Inspector", mostrando la información del actor seleccionado.
/// Actualmente permite editar la Transform (posición, rotación y escala) del actor.
/// </summary>
void UserInterface::drawInspector_()
{
  ImGui::Begin("Inspector");

  if (!m_selectedActor)
  {
    ImGui::Text("Ningun actor seleccionado.");
    ImGui::End();
    return;
  }

  ImGui::Text("Actor: %s", m_selectedActor->getName().c_str());
  ImGui::Separator();

  // Transform
  auto transform = m_selectedActor->getComponent<Transform>();

  if (!transform.isNull())
  {
    ImGui::Text("Transform");

    // Inicializar caché la primera vez que se muestra este actor
    if (!m_hasCachedTransform)
    {
      // Valores iniciales por defecto (por ahora, del Alien de prueba).
      m_cachedPos = EU::Vector3(0.0f, -1.0f, 6.0f);
      m_cachedRot = EU::Vector3(-1.0f, 3.0f, -0.10f);
      m_cachedScale = EU::Vector3(2.0f, 2.0f, 2.0f);
      m_hasCachedTransform = true;
    }

    bool changed = false;
    changed |= ImGui::DragFloat3("Position", &m_cachedPos.x, 0.1f);
    changed |= ImGui::DragFloat3("Rotation", &m_cachedRot.x, 0.1f);
    changed |= ImGui::DragFloat3("Scale", &m_cachedScale.x, 0.1f);

    // Si hay cambios, se actualiza el componente Transform del actor
    if (changed)
    {
      transform->setTransform(m_cachedPos, m_cachedRot, m_cachedScale);
    }
  }
  else
  {
    ImGui::Text("Sin componente Transform.");
  }

  ImGui::End();
}
