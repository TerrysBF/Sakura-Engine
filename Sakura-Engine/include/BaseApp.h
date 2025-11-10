#pragma once
#include "Prerequisites.h"
#include "Window.h"
#include "Device.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "Texture.h"
#include "RenderTargetView.h"
#include "DepthStencilView.h"
#include "Viewport.h"
#include "ShaderProgram.h"
#include "MeshComponent.h"
#include "Buffer.h"
#include "SamplerState.h"
#include "ObjReader.h"

class BaseApp {
public:
  BaseApp(HINSTANCE hInst, int nCmdShow);
  ~BaseApp() { destroy(); }

  int     run(HINSTANCE hInst, int nCmdShow);
  HRESULT init();
  void    update(float deltaTime);
  void    render();
  void    destroy();

private:
  static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

private:
  // D3D
  Window          m_window;
  Device          m_device;
  DeviceContext   m_deviceContext;
  SwapChain       m_swapChain;
  Texture         m_backBuffer;
  RenderTargetView m_renderTargetView;
  Texture         m_depthStencil;
  DepthStencilView m_depthStencilView;
  Viewport        m_viewport;

  ShaderProgram   m_shaderProgram;

  // Modelo
  ObjReader       m_modelLoader;     // <— loader correcto
  MeshComponent   m_mesh;
  Buffer          m_vertexBuffer;
  Buffer          m_indexBuffer;

  // Const buffers
  Buffer          m_cbNeverChanges;
  Buffer          m_cbChangeOnResize;
  Buffer          m_cbChangesEveryFrame;

  // Textura del modelo
  Texture         m_textureModel;    // (antes “m_textureCube”)
  SamplerState    m_samplerState;

  // Matrices
  XMMATRIX        m_World, m_View, m_Projection;
  XMFLOAT4        m_vMeshColor;

  // Copias CPU de CBs
  CBChangeOnResize    cbChangesOnResize;
  CBNeverChanges      cbNeverChanges;
  CBChangesEveryFrame cb;
};
