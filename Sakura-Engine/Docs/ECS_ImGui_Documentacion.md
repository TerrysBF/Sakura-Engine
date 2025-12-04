# Sakura-Engine – Diseño del ECS e Integración con ImGui

## 1. Objetivo del documento

Este documento describe el diseño del **ECS (Entity Component System)** implementado en Sakura-Engine y la integración de herramientas de edición en tiempo real mediante **Dear ImGui**.

Sirve como referencia para la materia de **Gráficas Computacionales 3D** y como guía rápida para entender cómo se organiza la escena, los componentes y la UI de edición.

---

## 2. Modelo ECS

### 2.1. Entidades

En Sakura-Engine las entidades se representan con la clase:

- **`Actor`**  
  Cada `Actor` es un objeto lógico de la escena (por ejemplo, el modelo Alien).  
  Internamente almacena un conjunto de componentes y expone métodos como:

```cpp
auto transform = actor->getComponent<Transform>();
auto mesh      = actor->getComponent<MeshComponent>();

Las entidades activas se guardan en un contenedor:
std::vector<EU::TSharedPointer<Actor>> m_actors;

2.2. Componentes

Los componentes encapsulan datos específicos de cada aspecto de la entidad.

Diagrama simple:

[Actor]
 ├── Transform
 ├── MeshComponent
 └── CameraComponent (conceptual / futuro)


* CameraComponent puede implementarse como componente dedicado o estar representado en la configuración de cámara manejada por BaseApp.

Ejemplo de uso de Transform en el código:

m_alien->getComponent<Transform>()->setTransform(
    EU::Vector3(0.0f, -1.0f, 6.0f),
    EU::Vector3(-1.0f, 3.0f, -0.10f),
    EU::Vector3(2.0f, 2.0f, 2.0f)
);

2.3. Sistemas

En este proyecto, los sistemas se implementan de forma sencilla dentro de BaseApp y clases auxiliares.

Sistema	Responsable	Función principal
Render System	BaseApp::render	Recorre m_actors y dibuja los que tienen MeshComponent.
Camera System	BaseApp	Configura matrices de vista/proyección antes del render.
UI System	UserInterface	Dibujar jerarquía, inspector y permitir edición en tiempo real via ImGui.

Ejemplo simplificado de sistema de render:

for (auto& actor : m_actors)
{
    if (!actor.isNull())
        actor->render(m_deviceContext);
}

3. Integración con ImGui

La clase UserInterface encapsula la lógica de Dear ImGui usando los backends:

imgui_impl_win32.h

imgui_impl_dx11.h

3.1. Inicialización
void UserInterface::init(void* hwnd,
                         ID3D11Device* device,
                         ID3D11DeviceContext* context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init((HWND)hwnd);
    ImGui_ImplDX11_Init(device, context);
}


BaseApp pasa el vector de actores a la UI:

m_ui.setSceneActors(&m_actors);

3.2. Panel de jerarquía (Hierarchy)

El panel de jerarquía muestra la lista de entidades activas:

for (auto& actorPtr : *m_actors)
{
    Actor* actor = actorPtr.get();
    if (!actor) continue;

    const std::string& nameStr = actor->getName();
    const char* visibleLabel = nameStr.empty() ? "Actor" : nameStr.c_str();

    ImGui::PushID(actor); // ID único
    bool selected = (m_selectedActor == actor);

    if (ImGui::Selectable(visibleLabel, selected))
    {
        m_selectedActor = actor;
        m_hasCachedTransform = false;
    }

    ImGui::PopID();
}


Al hacer clic en un actor, éste pasa a ser el actor seleccionado para el inspector.

3.3. Panel de inspector (Inspector)

El inspector muestra los componentes del actor seleccionado.
En esta versión, se edita principalmente el Transform:

auto transform = m_selectedActor->getComponent<Transform>();

if (!transform.isNull())
{
    bool changed = false;
    changed |= ImGui::DragFloat3("Position", &m_cachedPos.x, 0.1f);
    changed |= ImGui::DragFloat3("Rotation", &m_cachedRot.x, 0.1f);
    changed |= ImGui::DragFloat3("Scale",    &m_cachedScale.x, 0.1f);

    if (changed)
    {
        transform->setTransform(m_cachedPos, m_cachedRot, m_cachedScale);
    }
}

Esto permite edición en tiempo real del modelo Alien directamente desde la interfaz.

4. Flujo de ejecución (runtime)

Inicialización (BaseApp::init)

Crear ventana (Window).

Inicializar DirectX 11 (Device, DeviceContext, SwapChain, Viewport).

Cargar modelo y texturas (Model3D, MeshComponent, Texture).

Crear entidad Actor para el Alien y configurar su Transform.

Inicializar UserInterface y asociar m_actors.

Ciclo principal (BaseApp::run)

Mientras no se reciba WM_QUIT:

Procesar mensajes de Windows.

Llamar a update(deltaTime) y render().

Por frame

BaseApp::update:

Lógica de escena (si se añade).

m_ui.update() para construir la UI de ImGui.

BaseApp::render:

Limpiar backbuffer y depth buffer.

Dibujar escena 3D (Alien).

m_ui.render() para dibujar la interfaz.

Presentar el swapchain.
