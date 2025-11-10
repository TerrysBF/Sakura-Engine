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

/**
 * Clase principal de la aplicación.
 * Aquí se maneja la ventana, Direct3D y el ciclo de vida del render.
 */
class BaseApp {
public:
  /**
   * Constructor que recibe la instancia y cómo se muestra la ventana.
   * Realmente solo guarda lo necesario y luego run() hace el resto.
   */
  BaseApp(HINSTANCE hInst, int nCmdShow);

  /**
   * Destructor.
   * Llama a destroy() para liberar todos los recursos de D3D.
   */
  ~BaseApp() { destroy(); }

  /**
   * Función principal de ejecución.
   * Crea la ventana, inicializa Direct3D y entra al loop de mensajes/render.
   */
  int     run(HINSTANCE hInst, int nCmdShow);

  /**
   * Inicializa todos los recursos de Direct3D y carga el modelo.
   * Se llama una sola vez al inicio.
   */
  HRESULT init();

  /**
   * Actualiza la lógica por frame.
   * Recibe deltaTime para animaciones (por ejemplo rotar el modelo).
   */
  void    update(float deltaTime);

  /**
   * Dibuja la escena en pantalla.
   * Limpia el backbuffer, setea el pipeline y manda el DrawIndexed.
   */
  void    render();

  /**
   * Libera todos los recursos creados (buffers, shaders, swap chain, etc).
   * Se llama cuando se cierra la aplicación o en el destructor.
   */
  void    destroy();

private:
  /**
   * WndProc estática para manejar los mensajes de Windows.
   * Aquí se atiende WM_PAINT, WM_DESTROY, etc.
   */
  static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

private:
  // Ventana y objetos base de D3D
  Window          m_window;          // ventana Win32 donde se dibuja
  Device          m_device;          // ID3D11Device encapsulado
  DeviceContext   m_deviceContext;   // ID3D11DeviceContext inmediato
  SwapChain       m_swapChain;       // swap chain para presentar en pantalla
  Texture         m_backBuffer;      // textura del back buffer del swap chain
  RenderTargetView m_renderTargetView; // RTV que apunta al back buffer
  Texture         m_depthStencil;    // textura usada como depth/stencil
  DepthStencilView m_depthStencilView; // vista de depth/stencil para el OM
  Viewport        m_viewport;        // viewport donde se dibuja en la pantalla

  // Shaders
  ShaderProgram   m_shaderProgram;   // programa de shaders (VS+PS+input layout)

  // Modelo
  ObjReader       m_modelLoader;     // cargador de archivos .obj
  MeshComponent   m_mesh;            // datos de la malla (vértices e índices)
  Buffer          m_vertexBuffer;    // vertex buffer en GPU
  Buffer          m_indexBuffer;     // index buffer en GPU

  // Constant buffers
  Buffer          m_cbNeverChanges;      // CB para datos que casi no cambian (por ejemplo view)
  Buffer          m_cbChangeOnResize;    // CB para la proyección (cambia si cambia el tamaño)
  Buffer          m_cbChangesEveryFrame; // CB para cosas que se actualizan cada frame (mundo, color)

  // Textura del modelo y sampler
  Texture         m_textureModel;    // textura del modelo (Alien_Texture.png)
  SamplerState    m_samplerState;    // sampler para leer la textura desde el pixel shader

  // Matrices de transformación
  XMMATRIX        m_World;           // matriz mundo (posición/rotación/escala del modelo)
  XMMATRIX        m_View;            // matriz vista (cámara)
  XMMATRIX        m_Projection;      // matriz de proyección (perspectiva)
  XMFLOAT4        m_vMeshColor;      // color del mesh (si se quiere usar en el shader)

  // Copias en CPU de los datos de los constant buffers
  CBChangeOnResize    cbChangesOnResize;   // datos para el CB de proyección
  CBNeverChanges      cbNeverChanges;      // datos para el CB de vista
  CBChangesEveryFrame cb;                  // datos para el CB de mundo y color
};
