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
#include "Model3D.h"
#include "ECS/Actor.h"
#include "UserInterface.h"

/// Clase principal de la aplicación.
/// Administra la ventana, la inicialización de DirectX y el ciclo de render.
class
	BaseApp {
public:
	/// Constructor por defecto. No crea ni inicializa recursos gráficos.
	BaseApp() = default;

	/// Destructor. Llama a destroy() para liberar recursos.
	~BaseApp() { destroy(); }

	/// Ejecuta la aplicación: inicializa, entra al loop de mensajes y al ciclo principal.
	/// \param hInst Instancia de la aplicación.
	/// \param nCmdShow Modo de visualización de la ventana.
	/// \return Código de salida de la aplicación.
	int
		run(HINSTANCE hInst, int nCmdShow);

	/// Inicializa la ventana, el dispositivo gráfico y los recursos básicos.
	/// \return S_OK si todo salió bien; código de error en caso contrario.
	HRESULT
		init();

	/// Actualiza la lógica de la aplicación por frame.
	/// \param deltaTime Tiempo transcurrido desde el último frame.
	void
		update(float deltaTime);

	/// Renderiza la escena actual en el back buffer y la presenta en pantalla.
	void
		render();

	/// Libera y destruye los recursos creados por la aplicación.
	void
		destroy();

private:
	/// Procedimiento de ventana para procesar mensajes de Windows.
	/// \return Resultado del manejo del mensaje.
	static LRESULT CALLBACK
		WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	  UserInterface m_ui;
	// Ventana principal de la aplicación.
	Window                              m_window;

	// Dispositivo de Direct3D para creación de recursos.
	Device															m_device;

	// Contexto de dispositivo para emitir comandos de dibujo.
	DeviceContext												m_deviceContext;

	// Cadena de intercambio para presentar el back buffer.
	SwapChain                           m_swapChain;

	// Textura que representa el back buffer de la ventana.
	Texture                             m_backBuffer;

	// Vista de render asociada al back buffer.
	RenderTargetView									  m_renderTargetView;

	// Textura usada como depth-stencil buffer.
	Texture                             m_depthStencil;

	// Vista de depth-stencil para pruebas de profundidad y stencil.
	DepthStencilView									  m_depthStencilView;

	// Configuración del área de dibujo (viewport).
	Viewport                            m_viewport;

	// Programa de shaders usado para dibujar la escena.
	ShaderProgram												m_shaderProgram;

	//MeshComponent												m_mesh;
	//Buffer															m_vertexBuffer;
	//Buffer															m_indexBuffer;

	// Constant buffer con datos que casi no cambian.
	Buffer															m_cbNeverChanges;

	// Constant buffer con datos dependientes del tamaño de la ventana.
	Buffer															m_cbChangeOnResize;

	//Buffer															m_cbChangesEveryFrame;

	// Textura aplicada al modelo Alien.
	Texture 														m_Alien_Texture;

	//SamplerState												m_samplerState;

	//XMMATRIX                            m_World;

	// Matriz de vista (posición y orientación de la cámara).
	XMMATRIX                            m_View;

	// Matriz de proyección para transformar a espacio de clip.
	XMMATRIX                            m_Projection;

	//XMFLOAT4                            m_vMeshColor;// (0.7f, 0.7f, 0.7f, 1.0f);

	// Lista de actores presentes en la escena.
	std::vector<EU::TSharedPointer<Actor>> m_actors;

	// Actor principal que representa al modelo Alien.
	EU::TSharedPointer<Actor> m_alien;

	// Puntero al modelo 3D cargado.
	Model3D* m_model;

	// Estructura con datos de proyección y tamaño del viewport.
	CBChangeOnResize										cbChangesOnResize;

	// Estructura con datos constantes que casi no cambian.
	CBNeverChanges											cbNeverChanges;

	//CBChangesEveryFrame									cb;
};
