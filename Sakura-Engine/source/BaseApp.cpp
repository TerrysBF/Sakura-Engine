#include "BaseApp.h"
#include "ResourceManager.h"

// Para que el WndProc pueda pasarle eventos a ImGui
#include "imgui.h"
#include "backends/imgui_impl_win32.h"

// Declaración explícita del handler de Win32 para ImGui
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
  HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int
BaseApp::run(HINSTANCE hInst, int nCmdShow) {
  // Inicializar ventana (Window se encarga de registrar y crear el HWND)
  if (FAILED(m_window.init(hInst, nCmdShow, WndProc))) {
    return 0;
  }

  // Inicializar DirectX + recursos
  if (FAILED(init()))
    return 0;

  // Main message loop
  MSG msg = {};
  LARGE_INTEGER freq, prev;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&prev);

  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      LARGE_INTEGER curr;
      QueryPerformanceCounter(&curr);
      float deltaTime = static_cast<float>(curr.QuadPart - prev.QuadPart) / freq.QuadPart;
      prev = curr;

      update(deltaTime);
      render();
    }
  }
  return (int)msg.wParam;
}

HRESULT
BaseApp::init() {
  HRESULT hr = S_OK;

  // Crear swapchain
  hr = m_swapChain.init(m_device, m_deviceContext, m_backBuffer, m_window);

  if (FAILED(hr)) {
    ERROR("Main", "InitDevice",
      ("Failed to initialize SwpaChian. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear render target view
  hr = m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);

  if (FAILED(hr)) {
    ERROR("Main", "InitDevice",
      ("Failed to initialize RenderTargetView. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear textura de depth stencil
  hr = m_depthStencil.init(m_device,
    m_window.m_width,
    m_window.m_height,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    D3D11_BIND_DEPTH_STENCIL,
    4,
    0);

  if (FAILED(hr)) {
    ERROR("Main", "InitDevice",
      ("Failed to initialize DepthStencil. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear el depth stencil view
  hr = m_depthStencilView.init(m_device,
    m_depthStencil,
    DXGI_FORMAT_D24_UNORM_S8_UINT);

  if (FAILED(hr)) {
    ERROR("Main", "InitDevice",
      ("Failed to initialize DepthStencilView. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear el m_viewport
  hr = m_viewport.init(m_window);

  if (FAILED(hr)) {
    ERROR("Main", "InitDevice",
      ("Failed to initialize Viewport. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // ---------------------------------------------------------------------
  //  IMGUI: inicializar UserInterface (ventana + device + context)
  //  IMPORTANTE:
  //  - Si tu clase Window no tiene getHWND(), cambia esta línea por
  //    el miembro correcto (por ejemplo, m_window.m_hWnd).
  // ---------------------------------------------------------------------
  {
    HWND hwnd = m_window.getHWND(); // <-- CAMBIA a m_window.m_hWnd si no tienes getHWND()
    m_ui.init(
      (void*)hwnd,
      m_device.m_device,                 // ID3D11Device*
      m_deviceContext.m_deviceContext    // ID3D11DeviceContext*
    );
  }

  // ---------------------------------------------------------------------
  //  Carga de recursos: Modelo Alien + textura
  // ---------------------------------------------------------------------
  m_alien = EU::MakeShared<Actor>(m_device);

  if (!m_alien.isNull()) {
    std::vector<MeshComponent> alienMeshes;
    m_model = new Model3D("Alien.fbx", ModelType::FBX);
    alienMeshes = m_model->GetMeshes();

    std::vector<Texture> alienTextures;
    hr = m_Alien_Texture.init(m_device, "Alien_Texture", ExtensionType::PNG);
    if (FAILED(hr)) {
      ERROR("Main", "InitDevice",
        ("Failed to initialize Alien_Texture. HRESULT: " + std::to_string(hr)).c_str());
      return hr;
    }
    alienTextures.push_back(m_Alien_Texture);

    m_alien->setMesh(m_device, alienMeshes);
    m_alien->setTextures(alienTextures);
    m_alien->setName("Alien");
    m_actors.push_back(m_alien);
    m_ui.setSceneActors(&m_actors);
    m_alien->getComponent<Transform>()->setTransform(
      // Posición: un poco abajo y al fondo
      EU::Vector3(0.0f, -1.0f, 6.0f),
      // Rotación
      EU::Vector3(-1.0f, 3.0f, -0.10f),
      // Escala
      EU::Vector3(2.0f, 2.0f, 2.0f)
    );
  }
  else {
    ERROR("Main", "InitDevice", "Failed to create alien  Actor.");
    return E_FAIL;
  }

  // Define the input layout
  std::vector<D3D11_INPUT_ELEMENT_DESC> Layout;
  D3D11_INPUT_ELEMENT_DESC position;
  position.SemanticName = "POSITION";
  position.SemanticIndex = 0;
  position.Format = DXGI_FORMAT_R32G32B32_FLOAT;
  position.InputSlot = 0;
  position.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT /*0*/;
  position.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  position.InstanceDataStepRate = 0;
  Layout.push_back(position);

  D3D11_INPUT_ELEMENT_DESC texcoord;
  texcoord.SemanticName = "TEXCOORD";
  texcoord.SemanticIndex = 0;
  texcoord.Format = DXGI_FORMAT_R32G32_FLOAT;
  texcoord.InputSlot = 0;
  texcoord.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT /*0*/;
  texcoord.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  texcoord.InstanceDataStepRate = 0;
  Layout.push_back(texcoord);

  // Create the Shader Program
  hr = m_shaderProgram.init(m_device, "Sakura-Engine.fx", Layout);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice",
      ("Failed to initialize ShaderProgram. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Create the constant buffers
  hr = m_cbNeverChanges.init(m_device, sizeof(CBNeverChanges));
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice",
      ("Failed to initialize NeverChanges Buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  hr = m_cbChangeOnResize.init(m_device, sizeof(CBChangeOnResize));
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice",
      ("Failed to initialize ChangeOnResize Buffer. HRESULT: " + std::to_string(hr)).c_str());
    return hr;
  }

  // Initialize the view matrix
  // Cámara más cerca y mirando al centro del alien
  XMVECTOR Eye = XMVectorSet(0.0f, 1.5f, -4.0f, 0.0f);
  XMVECTOR At = XMVectorSet(0.0f, 0.7f, 0.0f, 0.0f);
  XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

  m_View = XMMatrixLookAtLH(Eye, At, Up);

  m_Projection = XMMatrixPerspectiveFovLH(
    XM_PIDIV4,
    m_window.m_width / (FLOAT)m_window.m_height,
    0.01f,
    100.0f
  );

  // Initialize the projection matrix
  cbNeverChanges.mView = XMMatrixTranspose(m_View);
  m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_window.m_width / (FLOAT)m_window.m_height, 0.01f, 100.0f);
  cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);

  return S_OK;
}

void BaseApp::update(float deltaTime)
{
  // Update our time
  static float t = 0.0f;
  if (m_swapChain.m_driverType == D3D_DRIVER_TYPE_REFERENCE)
  {
    t += (float)XM_PI * 0.0125f;
  }
  else
  {
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if (dwTimeStart == 0)
      dwTimeStart = dwTimeCur;
    t = (dwTimeCur - dwTimeStart) / 1000.0f;
  }

  // Actualizar la matriz de proyección y vista
  cbNeverChanges.mView = XMMatrixTranspose(m_View);
  m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);

  m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_window.m_width / (FLOAT)m_window.m_height, 0.01f, 100.0f);
  cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);
  m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);

  // Update Actors
  for (auto& actor : m_actors) {
    actor->update(deltaTime, m_deviceContext);
  }

  // ------------------------------------------------
  // IMGUI: construir la UI (ventanas, dockspace, etc.)
  // ------------------------------------------------
  m_ui.update();
}

void
BaseApp::render() {
  // Set Render Target View
  float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
  m_renderTargetView.render(m_deviceContext, m_depthStencilView, 1, ClearColor);

  // Set Viewport
  m_viewport.render(m_deviceContext);

  // Set depth stencil view
  m_depthStencilView.render(m_deviceContext);

  // Set shader program
  m_shaderProgram.render(m_deviceContext);

  // Asignar buffers constantes
  m_cbNeverChanges.render(m_deviceContext, 0, 1);
  m_cbChangeOnResize.render(m_deviceContext, 1, 1);

  // Render all actors
  for (auto& actor : m_actors) {
    actor->render(m_deviceContext);
  }

  // ------------------------------------------------
  // IMGUI: dibujar la UI sobre el backbuffer actual
  // ------------------------------------------------
  m_ui.render();

  // Present our back buffer to our front buffer
  m_swapChain.present();
}

void
BaseApp::destroy() {
  // Primero destruir UI de ImGui (antes de destruir device/context)
  m_ui.destroy();

  if (m_deviceContext.m_deviceContext) m_deviceContext.m_deviceContext->ClearState();

  m_cbNeverChanges.destroy();
  m_cbChangeOnResize.destroy();
  m_shaderProgram.destroy();
  m_depthStencil.destroy();
  m_depthStencilView.destroy();
  m_renderTargetView.destroy();
  m_swapChain.destroy();
  m_backBuffer.destroy();
  m_deviceContext.destroy();
  m_device.destroy();
}

LRESULT
BaseApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  // ------------------------------------------------
  // Pasar input a ImGui (mouse / teclado, etc.)
  // Cuidado: WndProc se llama ANTES de init(), por eso
  // se verifica que exista un contexto.
  // ------------------------------------------------
  if (ImGui::GetCurrentContext() != nullptr)
  {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
      return true;
  }

  switch (message)
  {
  case WM_CREATE:
  {
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
  }
  return 0;

  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
  }
  return 0;

  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}
