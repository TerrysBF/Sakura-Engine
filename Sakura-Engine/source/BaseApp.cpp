// Comentarios básicos estilo estudiante :)

#include "BaseApp.h"
#include <string>
#include <vector>
#include <windows.h>

// Variables globales para guardar los nombres de los archivos que se usan.
// Aquí se indican el fx, el modelo (FBX) y la textura base.
static const wchar_t* PROJECT_TAG = L"[WildvineEngine] ";
static const char* FX_NAME = "Sakura-Engine.fx";
static const wchar_t* OBJ_NAME = L"Alien.fbx";
// static const wchar_t* MTL_NAME = L"Alien.mtl";
static const char* TEX_BASE = "Alien_Texture"; // nombre sin extensión

// Función auxiliar para revisar si un archivo existe en disco.
// Recibe una ruta en wchar_t y regresa true si el archivo está presente.
static bool FileExistsW(const wchar_t* path) {
  DWORD attr = GetFileAttributesW(path);
  return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

// Constructor.
// Inicializamos también el recurso Model3D con nombre y tipo FBX.
BaseApp::BaseApp(HINSTANCE, int)
  : m_model("Alien", ModelType::FBX) {
}

// Función principal del ciclo de la aplicación.
// Aquí se procesa la ventana, se actualiza y se dibuja cada frame.
int BaseApp::run(HINSTANCE hInst, int nCmdShow) {
  // Crear la ventana con el WndProc de esta clase.
  if (FAILED(m_window.init(hInst, nCmdShow, WndProc))) return 0;
  // Inicializar Direct3D y todos los recursos.
  if (FAILED(init())) return 0;

  MSG msg = {};
  LARGE_INTEGER freq, prev;
  // Se usa el contador de alto rendimiento para medir deltaTime.
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&prev);

  // Bucle principal: sale cuando llega el mensaje WM_QUIT.
  while (WM_QUIT != msg.message) {
    // Si hay mensajes de Windows pendientes, se procesan.
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      // Si no hay mensajes, se actualiza y se renderiza la escena.
      LARGE_INTEGER curr;
      QueryPerformanceCounter(&curr);
      float dt = float(curr.QuadPart - prev.QuadPart) / float(freq.QuadPart);
      prev = curr;

      update(dt);
      render();
    }
  }
  return (int)msg.wParam;
}

// Esta función crea e inicializa todos los recursos de Direct3D.
// Aquí se prepara el swap chain, el render target, el depth buffer,
// los shaders, buffers de vértices, texturas y constantes.
HRESULT BaseApp::init() {
  HRESULT hr = S_OK;

  // Crear el swap chain y obtener el backbuffer como textura.
  hr = m_swapChain.init(m_device, m_deviceContext, m_backBuffer, m_window);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed SwapChain. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear la Render Target View a partir del backbuffer.
  hr = m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed RTV. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear la textura de profundidad usando la misma configuración de MSAA
  // que el swap chain, para que ambas coincidan.
  {
    unsigned int sampleCount = m_swapChain.getSampleCount();
    unsigned int qualityLevels = m_swapChain.getQualityLevels();

    // Ajuste del nivel de calidad para que sea igual que el usado en el swap chain.
    if (sampleCount > 1 && qualityLevels > 0)
      qualityLevels -= 1;
    else
      qualityLevels = 0;

    hr = m_depthStencil.init(
      m_device,
      m_window.m_width,
      m_window.m_height,
      DXGI_FORMAT_D24_UNORM_S8_UINT,
      D3D11_BIND_DEPTH_STENCIL,
      sampleCount,
      qualityLevels
    );
  }

  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed Depth. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear la vista de depth/stencil a partir de la textura creada arriba.
  hr = m_depthStencilView.init(m_device, m_depthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed DSV. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear el viewport usando el tamaño de la ventana.
  hr = m_viewport.init(m_window);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed Viewport. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // Configurar un rasterizer state sin culling para poder ver las caras
  // aunque el orden de los vértices esté al revés.
  {
    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;   // Se dibujan triángulos sólidos.
    rd.CullMode = D3D11_CULL_NONE;    // No se descarta ninguna cara.
    rd.FrontCounterClockwise = FALSE; // Front face por defecto (clockwise).
    rd.DepthClipEnable = TRUE;        // Se respeta el rango de profundidad.

    ID3D11RasterizerState* rs = nullptr;
    HRESULT hrRS = m_device.m_device->CreateRasterizerState(&rd, &rs);
    if (FAILED(hrRS)) {
      ERROR("Main", "InitDevice", ("Failed RasterizerState. HR:" + std::to_string(hrRS)).c_str());
      return hrRS;
    }
    // Se aplica el estado al pipeline.
    m_deviceContext.m_deviceContext->RSSetState(rs);
    // Se libera la referencia local, el contexto ya lo mantiene internamente.
    rs->Release();
  }

  // Mostrar en el output la ruta actual de trabajo, para saber desde dónde se cargan los archivos.
  wchar_t cwd[MAX_PATH];
  GetCurrentDirectoryW(MAX_PATH, cwd);
  OutputDebugStringW((std::wstring(PROJECT_TAG) + L"CWD = " + cwd + L"\n").c_str());

  // Comprobar que el archivo FBX existe.
  if (!FileExistsW(OBJ_NAME)) {
    MessageBoxW(nullptr, (std::wstring(L"No existe ") + OBJ_NAME).c_str(), PROJECT_TAG, MB_ICONERROR);
    return E_FAIL;
  }

  // Crear el input layout que describe cómo están formados los vértices.
  // En este caso cada vértice tiene posición (POSITION) y coordenadas de textura (TEXCOORD).
  std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
  D3D11_INPUT_ELEMENT_DESC e{};
  e.SemanticIndex = 0;
  e.InputSlot = 0;
  e.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  e.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  e.InstanceDataStepRate = 0;

  e.SemanticName = "POSITION";
  e.Format = DXGI_FORMAT_R32G32B32_FLOAT;
  layout.push_back(e);

  e.SemanticName = "TEXCOORD";
  e.Format = DXGI_FORMAT_R32G32_FLOAT;
  layout.push_back(e);

  // Inicializar el programa de shaders (vertex shader, pixel shader e input layout).
  hr = m_shaderProgram.init(m_device, FX_NAME, layout);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed ShaderProgram. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // -------------------------------------------------------------------
  // Cargar el modelo usando el sistema de recursos (Model3D)
  // -------------------------------------------------------------------
  if (!m_model.load("Alien")) {
    MessageBoxW(nullptr, L"Falló la carga de Alien.fbx (Model3D)", PROJECT_TAG, MB_ICONERROR);
    return E_FAIL;
  }

  const auto& meshes = m_model.GetMeshes();
  if (meshes.empty()) {
    MessageBoxW(nullptr, L"Model3D no tiene mallas cargadas", PROJECT_TAG, MB_ICONERROR);
    return E_FAIL;
  }

  // Por ahora usamos solo la primera malla (igual que antes con un solo MeshComponent).
  m_mesh = meshes[0];

  // Mensaje de depuración con el número de vértices e índices cargados.
  {
    char buf[256];
    sprintf_s(buf, "[Wildvine] verts=%d idx=%d\n",
      (int)m_mesh.m_numVertex, (int)m_mesh.m_numIndex);
    OutputDebugStringA(buf);
  }

  // Crear el vertex buffer en GPU usando los datos de m_mesh.
  hr = m_vertexBuffer.init(m_device, m_mesh, D3D11_BIND_VERTEX_BUFFER);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed VertexBuffer. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear el index buffer en GPU usando los índices de m_mesh.
  hr = m_indexBuffer.init(m_device, m_mesh, D3D11_BIND_INDEX_BUFFER);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed IndexBuffer. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // Configurar la topología de dibujo como lista de triángulos.
  m_deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Cargar la textura del modelo y crear el shader resource view.
  hr = m_textureModel.init(m_device, TEX_BASE, ExtensionType::PNG);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed Texture PNG (Alien_Texture). HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // Crear el sampler state que se usará en el pixel shader.
  hr = m_samplerState.init(m_device);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed SamplerState. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // -------------------------------------------------------------------
  // Matrices base
  // -------------------------------------------------------------------

  // World: posición/orientación del modelo. Por ahora identidad; en update la animamos.
  m_World = XMMatrixIdentity();

  // Cámara un poco más lejos y arriba para ver mejor el modelo FBX.
  XMVECTOR Eye = XMVectorSet(0.0f, 2.5f, -8.0f, 0.0f);
  XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

  m_View = XMMatrixLookAtLH(Eye, At, Up);

  m_Projection = XMMatrixPerspectiveFovLH(
    XM_PIDIV4,
    m_window.m_width / (FLOAT)m_window.m_height,
    0.01f,
    100.0f
  );

  // Se llenan las estructuras de los constant buffers con las matrices transpuestas.
  cbNeverChanges.mView = XMMatrixTranspose(m_View);
  cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);
  cb.mWorld = XMMatrixTranspose(m_World);
  cb.vMeshColor = XMFLOAT4(1, 1, 1, 1); // Color blanco para el mesh.

  // Crear los constant buffers en GPU.
  if (FAILED(m_cbNeverChanges.init(m_device, sizeof(CBNeverChanges))))          return E_FAIL;
  if (FAILED(m_cbChangeOnResize.init(m_device, sizeof(CBChangeOnResize))))      return E_FAIL;
  if (FAILED(m_cbChangesEveryFrame.init(m_device, sizeof(CBChangesEveryFrame))))return E_FAIL;

  // Subir el contenido inicial de los constant buffers a la GPU.
  m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);
  m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);
  m_cbChangesEveryFrame.update(m_deviceContext, nullptr, 0, nullptr, &cb, 0, 0);

  return S_OK;
}

// En update se actualiza la lógica por frame.
// Aquí se rota el modelo y se actualizan de nuevo las matrices en los constant buffers.
void BaseApp::update(float dt) {
  // t acumula el tiempo para animar la rotación.
  static float t = 0.f;
  t += dt;

  // --- Transform del modelo FBX ---

  // Escala global del modelo (ajusta este valor si se ve muy grande/pequeño).
  const float s = 2.0f; // tú ya lo tenías así
  XMMATRIX S = XMMatrixScaling(s, s, s);

  // Corrección de orientación del FBX:
  // Blender/FBX suele exportar Z-up, y DirectX es Y-up.
  // Esto rota -90° en X para "pararlo".
  XMMATRIX Rfix = XMMatrixRotationX(-XM_PIDIV2);

  // Rotación suave en el eje Y para que gire.
  XMMATRIX Rspin = XMMatrixRotationY(t * 0.5f);

  // Levantar un poco el modelo si queda muy abajo en pantalla.
  XMMATRIX T = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

  // Orden típico: escala -> corrección eje -> rotación animada -> traslación.
  m_World = S * Rfix * Rspin * T;

  // Actualizar el constant buffer de per-frame.
  cb.mWorld = XMMatrixTranspose(m_World);
  m_cbChangesEveryFrame.update(m_deviceContext, nullptr, 0, nullptr, &cb, 0, 0);

  // View/Projection se recalculan por si cambia el tamaño de la ventana.
  cbNeverChanges.mView = XMMatrixTranspose(m_View);
  m_Projection = XMMatrixPerspectiveFovLH(
    XM_PIDIV4,
    m_window.m_width / (FLOAT)m_window.m_height,
    0.01f,
    100.0f
  );
  cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);
  m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);
  m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);
}


// En render se limpia la pantalla, se configura el pipeline y se dibuja el modelo.
void BaseApp::render() {
  // Color con el que se limpia el render target.
  float ClearColor[4] = { 0.10f, 0.10f, 0.12f, 1.0f };

  // Limpiar y hacer bind de RTV y DSV.
  m_renderTargetView.render(m_deviceContext, m_depthStencilView, 1, ClearColor);
  // Aplicar el viewport al contexto.
  m_viewport.render(m_deviceContext);
  // Limpiar el depth buffer.
  m_depthStencilView.render(m_deviceContext);

  // Activar vertex shader, pixel shader e input layout.
  m_shaderProgram.render(m_deviceContext);

  // Enlazar el vertex buffer y el index buffer.
  m_vertexBuffer.render(m_deviceContext, 0, 1);
  m_indexBuffer.render(m_deviceContext, 0, 1, false, DXGI_FORMAT_R32_UINT);

  // Enlazar los constant buffers necesarios en VS y PS.
  m_cbNeverChanges.render(m_deviceContext, 0, 1);
  m_cbChangeOnResize.render(m_deviceContext, 1, 1);
  m_cbChangesEveryFrame.render(m_deviceContext, 2, 1);
  m_cbChangesEveryFrame.render(m_deviceContext, 2, 1, true); // también para el pixel shader.

  // Enlazar la textura del modelo y el sampler al pixel shader.
  m_textureModel.render(m_deviceContext, 0, 1);
  m_samplerState.render(m_deviceContext, 0, 1);

  // Dibujar el modelo usando índices.
  m_deviceContext.DrawIndexed(m_mesh.m_numIndex, 0, 0);

  // Presentar el backbuffer en pantalla.
  m_swapChain.present();
}

// Esta función se encarga de liberar los recursos al cerrar la aplicación.
void BaseApp::destroy() {
  // Limpiar el estado del pipeline antes de destruir.
  if (m_deviceContext.m_deviceContext)
    m_deviceContext.m_deviceContext->ClearState();

  m_samplerState.destroy();
  m_textureModel.destroy();

  m_cbNeverChanges.destroy();
  m_cbChangeOnResize.destroy();
  m_cbChangesEveryFrame.destroy();
  m_vertexBuffer.destroy();
  m_indexBuffer.destroy();
  m_shaderProgram.destroy();

  m_depthStencilView.destroy();
  m_depthStencil.destroy();
  m_renderTargetView.destroy();
  m_backBuffer.destroy();
  m_swapChain.destroy();
  m_deviceContext.destroy();
  m_device.destroy();
}

// Procedimiento de ventana básico.
// Aquí se manejan los mensajes principales de la ventana (crear, pintar y cerrar).
LRESULT BaseApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE: {
    // Guardar un puntero a la app en la ventana si se quisiera usar después.
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
  } return 0;

  case WM_PAINT: {
    // Manejo básico de repintado, aquí no se dibuja nada extra.
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
  } return 0;

  case WM_DESTROY:
    // Cuando la ventana se cierra se manda WM_QUIT para salir del bucle.
    PostQuitMessage(0);
    return 0;
  }

  // Si no se manejó el mensaje, se pasa al procedimiento por defecto.
  return DefWindowProc(hWnd, message, wParam, lParam);
}
