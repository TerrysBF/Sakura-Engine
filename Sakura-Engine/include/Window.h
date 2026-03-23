#pragma once
#include "Prerequisites.h"

/**
 * @class Window
 * @brief Se encarga de crear y gestionar la ventana de Windows (Win32).
 * * Es el "lienzo" o marco de fotos donde nuestro motor gráfico va a pintar.
 * Sin esta clase, no veríamos nada en el monitor porque no habría un espacio asignado por el sistema operativo.
 */
class BaseApp;
class
	Window {
public:
	Window() = default;
	~Window() = default;

	/**
	 * @brief Registra y abre la ventana en el escritorio de Windows.
	 * @param hInstance El "ID" de proceso que Windows le da a tu programa.
	 * @param nCmdShow Indica si la ventana inicia maximizada, minimizada o normal.
	 * @param wndproc La función "oído" que escucha eventos (clics, teclas, cerrar ventana).
	 * @return S_OK si Windows permitió crear la ventana correctamente.
	 */
	HRESULT
		init(HINSTANCE hInstance, int nCmdShow, WNDPROC wndproc, BaseApp* app);

	/**
	 * @brief Revisa si han ocurrido cambios en la ventana (como si el usuario la movió).
	 */
	void
		update();

	/**
	 * @brief Prepara la ventana para que DirectX pueda dibujar en ella.
	 */
	void
		render();

	void
		destroy();

public:
	HWND m_hWnd = nullptr;
	unsigned int m_width;
	unsigned int m_height;
private:
	HINSTANCE m_hInst = nullptr;
	RECT m_rect;
	std::string m_windowName = "Wildvine Engine";
};