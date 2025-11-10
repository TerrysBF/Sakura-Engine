// BaseApp.cpp  (parche opción 2: desactivar culling para probar winding)
// Comentarios básicos estilo estudiante :)

#include "BaseApp.h"
#include <string>
#include <vector>
#include <windows.h>
#include "ObjReader.h"

// ==== Config de tu proyecto / assets ====
static const wchar_t* PROJECT_TAG = L"[WildvineEngine] ";
static const char* FX_NAME = "Sakura-Engine.fx";
static const wchar_t* OBJ_NAME = L"Alien.obj";
static const wchar_t* MTL_NAME = L"Alien.mtl";
static const char* TEX_BASE = "Alien_Texture"; // sin extensión
// ========================================

// Función simple para checar si existe un archivo (wide)
static bool FileExistsW(const wchar_t* path) {
  DWORD attr = GetFileAttributesW(path);
  return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

BaseApp::BaseApp(HINSTANCE, int)
{}

// Loop principal de la app (windows messages + update/render)
int BaseApp::run(HINSTANCE hInst, int nCmdShow) {
  if (FAILED(m_window.init(hInst, nCmdShow, WndProc))) return 0;
  if (FAILED(init())) return 0;

  MSG msg = {};
  LARGE_INTEGER freq, prev; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&prev);

  while (WM_QUIT != msg.message) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg); DispatchMessage(&msg);
    }
    else {
      LARGE_INTEGER curr; QueryPerformanceCounter(&curr);
      float dt = float(curr.QuadPart - prev.QuadPart) / float(freq.QuadPart);
      prev = curr;
      update(dt);
      render();
    }
  }
  return (int)msg.wParam;
}

HRESULT BaseApp::init() {
  HRESULT hr = S_OK;

  // 1) SwapChain + backbuffer
  hr = m_swapChain.init(m_device, m_deviceContext, m_backBuffer, m_window);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed SwapChain. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // 2) Render Target View (RTV)
  hr = m_renderTargetView.init(m_device, m_backBuffer, DXGI_FORMAT_R8G8B8A8_UNORM);
  if (FAILED(hr)) {
    ERROR("Main", "InitDevice", ("Failed RTV. HR:" + std::to_string(hr)).c_str());
    return hr;
  }

  // 3) Depth + DepthStencilView (DSV) usando mismo MSAA que el swap chain
  {
    unsigned int sampleCount = m_swapChain.getSampleCount();
    unsigned int qualityLevels = m_swapChain.getQualityLevels();

    // Igual que en el swap chain: qualityLevels - 1 (si aplica)
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

  hr = m_depthStencilView.init(m_device, m_depthStencil, DXGI_FORMAT_D24_UNORM_S8_UINT);
  if (FAILED(hr)) { ERROR("Main", "InitDevice", ("Failed DSV. HR:" + std::to_string(hr)).c_str()); return hr; }

  // 4) Viewport
  hr = m_viewport.init(m_window);
  if (FAILED(hr)) { ERROR("Main", "InitDevice", ("Failed Viewport. HR:" + std::to_string(hr)).c_str()); return hr; }

  // === PARCHE OPCIÓN 2: Rasterizer sin culling (para probar winding) ===
  {
    D3D11_RASTERIZER_DESC rd{};
    rd.FillMode = D3D11_FILL_SOLID;   // pon D3D11_FILL_WIREFRAME si quieres ver la malla
    rd.CullMode = D3D11_CULL_NONE;    // clave: sin culling (descarta problema de winding)
    rd.FrontCounterClockwise = FALSE; // si no ves nada, prueba TRUE temporalmente
    rd.DepthClipEnable = TRUE;

    ID3D11RasterizerState* rs = nullptr;
    HRESULT hrRS = m_device.m_device->CreateRasterizerState(&rd, &rs);
    if (FAILED(hrRS)) {
      ERROR("Main", "InitDevice", ("Failed RasterizerState. HR:" + std::to_string(hrRS)).c_str());
      return hrRS;
    }
    m_deviceContext.m_deviceContext->RSSetState(rs);
    rs->Release(); // ya está seteado en el pipeline
  }
  // === Fin parche rasterizer ===

  // ---- Log CWD y verificación de archivos ----
  wchar_t cwd[MAX_PATH]; GetCurrentDirectoryW(MAX_PATH, cwd);
  OutputDebugStringW((std::wstring(PROJECT_TAG) + L"CWD = " + cwd + L"\n").c_str());

  if (!FileExistsW(OBJ_NAME)) { MessageBoxW(nullptr, (std::wstring(L"No existe ") + OBJ_NAME).c_str(), PROJECT_TAG, MB_ICONERROR); return E_FAIL; }
  if (!FileExistsW(MTL_NAME)) { MessageBoxW(nullptr, (std::wstring(L"No existe ") + MTL_NAME).c_str(), PROJECT_TAG, MB_ICONWARNING); /* MTL opcional */ }

  // ---- Input layout: lo dejo como lo tenías (POSITION + TEXCOORD) ----
  std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
  D3D11_INPUT_ELEMENT_DESC e{};
  e.SemanticIndex = 0; e.InputSlot = 0; e.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  e.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; e.InstanceDataStepRate = 0;

  e.SemanticName = "POSITION"; e.Format = DXGI_FORMAT_R32G32B32_FLOAT; layout.push_back(e);
  e.SemanticName = "TEXCOORD"; e.Format = DXGI_FORMAT_R32G32_FLOAT;    layout.push_back(e);

  // Shader (fx)
  hr = m_shaderProgram.init(m_device, FX_NAME, layout);
  if (FAILED(hr)) { ERROR("Main", "InitDevice", ("Failed ShaderProgram. HR:" + std::to_string(hr)).c_str()); return hr; }

  // ---- Cargar OBJ a CPU (m_mesh) ----
  if (!m_modelLoader.load("Alien", m_mesh)) {
    MessageBoxW(nullptr, L"Falló la carga de Alien.obj", L"WildvineEngine", MB_ICONERROR);
    return E_FAIL;
  }

  // (debug) Log de conteos de malla
  {
    char buf[256];
    sprintf_s(buf, "[Wildvine] verts=%d idx=%d\n", (int)m_mesh.m_numVertex, (int)m_mesh.m_numIndex);
    OutputDebugStringA(buf);
  }

  // ---- Buffers GPU desde m_mesh ----
  hr = m_vertexBuffer.init(m_device, m_mesh, D3D11_BIND_VERTEX_BUFFER);
  if (FAILED(hr)) { ERROR("Main", "InitDevice", ("Failed VertexBuffer. HR:" + std::to_string(hr)).c_str()); return hr; }

  hr = m_indexBuffer.init(m_device, m_mesh, D3D11_BIND_INDEX_BUFFER);
  if (FAILED(hr)) { ERROR("Main", "InitDevice", ("Failed IndexBuffer. HR:" + std::to_string(hr)).c_str()); return hr; }

  // Topología triángulos
  m_deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // ---- Textura y sampler ----
  hr = m_textureModel.init(m_device, TEX_BASE, ExtensionType::PNG);
  if (FAILED(hr)) { ERROR("Main", "InitDevice", ("Failed Texture PNG (Alien_Texture). HR:" + std::to_string(hr)).c_str()); return hr; }

  hr = m_samplerState.init(m_device);
  if (FAILED(hr)) { ERROR("Main", "InitDevice", ("Failed SamplerState. HR:" + std::to_string(hr)).c_str()); return hr; }

  // ---- Matrices mínimas (World, View, Proj) ----
  m_World = XMMatrixIdentity();
  XMVECTOR Eye = XMVectorSet(0.0f, 1.5f, -4.0f, 0.0f);
  XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
  m_View = XMMatrixLookAtLH(Eye, At, Up);
  m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_window.m_width / (FLOAT)m_window.m_height, 0.01f, 100.0f);

  cbNeverChanges.mView = XMMatrixTranspose(m_View);
  cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);
  cb.mWorld = XMMatrixTranspose(m_World);
  cb.vMeshColor = XMFLOAT4(1, 1, 1, 1);

  // Const buffers
  if (FAILED(m_cbNeverChanges.init(m_device, sizeof(CBNeverChanges))))      return E_FAIL;
  if (FAILED(m_cbChangeOnResize.init(m_device, sizeof(CBChangeOnResize))))  return E_FAIL;
  if (FAILED(m_cbChangesEveryFrame.init(m_device, sizeof(CBChangesEveryFrame)))) return E_FAIL;

  // Subir una vez a GPU
  m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);
  m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);
  m_cbChangesEveryFrame.update(m_deviceContext, nullptr, 0, nullptr, &cb, 0, 0);

  return S_OK;
}

void BaseApp::update(float dt) {
  static float t = 0.f; t += dt;
  m_World = XMMatrixRotationY(t * 0.5f);
  cb.mWorld = XMMatrixTranspose(m_World);
  m_cbChangesEveryFrame.update(m_deviceContext, nullptr, 0, nullptr, &cb, 0, 0);

  cbNeverChanges.mView = XMMatrixTranspose(m_View);
  m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, m_window.m_width / (FLOAT)m_window.m_height, 0.01f, 100.0f);
  cbChangesOnResize.mProjection = XMMatrixTranspose(m_Projection);
  m_cbNeverChanges.update(m_deviceContext, nullptr, 0, nullptr, &cbNeverChanges, 0, 0);
  m_cbChangeOnResize.update(m_deviceContext, nullptr, 0, nullptr, &cbChangesOnResize, 0, 0);
}

void BaseApp::render() {
  float ClearColor[4] = { 0.10f, 0.10f, 0.12f, 1.0f };
  m_renderTargetView.render(m_deviceContext, m_depthStencilView, 1, ClearColor);
  m_viewport.render(m_deviceContext);
  m_depthStencilView.render(m_deviceContext);

  m_shaderProgram.render(m_deviceContext);

  m_vertexBuffer.render(m_deviceContext, 0, 1);
  m_indexBuffer.render(m_deviceContext, 0, 1, false, DXGI_FORMAT_R32_UINT);

  m_cbNeverChanges.render(m_deviceContext, 0, 1);
  m_cbChangeOnResize.render(m_deviceContext, 1, 1);
  m_cbChangesEveryFrame.render(m_deviceContext, 2, 1);
  m_cbChangesEveryFrame.render(m_deviceContext, 2, 1, true);

  m_textureModel.render(m_deviceContext, 0, 1);
  m_samplerState.render(m_deviceContext, 0, 1);

  m_deviceContext.DrawIndexed(m_mesh.m_numIndex, 0, 0);

  m_swapChain.present();
}

void BaseApp::destroy() {
  if (m_deviceContext.m_deviceContext) m_deviceContext.m_deviceContext->ClearState();

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

// WinProc básico
LRESULT BaseApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CREATE: {
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
  } return 0;
  case WM_PAINT: {
    PAINTSTRUCT ps; BeginPaint(hWnd, &ps); EndPaint(hWnd, &ps);
  } return 0;
  case WM_DESTROY: PostQuitMessage(0); return 0;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}
