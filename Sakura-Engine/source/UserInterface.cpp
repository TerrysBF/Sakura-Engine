#include "UserInterface.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Si necesitas más includes del motor, agrégalos aquí.

void UserInterface::init(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Navegación con teclado
  // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Si luego quieres docking

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init((HWND)hwnd);
  ImGui_ImplDX11_Init(device, context);
}

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

void UserInterface::update()
{
  // Comenzar nuevo frame de ImGui
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // Docking opcional (lo dejamos apagado para evitar problemas de versión)
  // ImGuiViewport* vp = ImGui::GetMainViewport();
  // ImGui::DockSpaceOverViewport(vp);

  drawMainMenuBar_();
  drawHierarchy_();
  drawInspector_();
}

void UserInterface::render()
{
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::destroy()
{
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

// -----------------------------------------------------------------------------
// Ventanas internas
// -----------------------------------------------------------------------------

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
    // Label visible: si no tiene nombre, mostramos "Actor"
    const char* visibleLabel = nameStr.empty() ? "Actor" : nameStr.c_str();

    // ID único basado en el puntero del actor (evita problemas de IDs vacíos)
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

  // ---------------------------------------------------------------------
  // Transform (edición en tiempo real)
  // ---------------------------------------------------------------------
  auto transform = m_selectedActor->getComponent<Transform>();

  if (!transform.isNull())
  {
    ImGui::Text("Transform");

    // Inicializar cache la primera vez que mostramos este actor
    if (!m_hasCachedTransform)
    {
      // Si tu Transform tiene getters, puedes usarlos aquí.
      // Por ahora reutilizamos valores iniciales del Alien.
      m_cachedPos = EU::Vector3(0.0f, -1.0f, 6.0f);
      m_cachedRot = EU::Vector3(-1.0f, 3.0f, -0.10f);
      m_cachedScale = EU::Vector3(2.0f, 2.0f, 2.0f);
      m_hasCachedTransform = true;
    }

    bool changed = false;
    changed |= ImGui::DragFloat3("Position", &m_cachedPos.x, 0.1f);
    changed |= ImGui::DragFloat3("Rotation", &m_cachedRot.x, 0.1f);
    changed |= ImGui::DragFloat3("Scale", &m_cachedScale.x, 0.1f);

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
