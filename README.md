# Sakura-Engine
Este es un compendio de las clases vistas en las materia de Graficas Computacionales 3D, Generación 2026 -01

Este proyecto implementa una base para un motor gráfico utilizando **DirectX 11**.  
Se organiza en clases independientes que encapsulan recursos y procesos como ventanas, dispositivos, contextos de render y manejo de texturas.  
El objetivo principal es mostrar una arquitectura modular, con documentación Doxygen y buenas prácticas de liberación de memoria.

---

## 📌 Clases principales

- **Window**  
  Encapsula la creación y gestión de la ventana de la aplicación. Define el área de cliente y mantiene dimensiones de ancho y alto.

- **Device**  
  Maneja el dispositivo principal de Direct3D. Se encarga de crear recursos gráficos como buffers, shaders, texturas y vistas de render.

- **DeviceContext**  
  Representa el contexto de ejecución del dispositivo. Administra el pipeline gráfico, asigna buffers, shaders y ejecuta comandos de dibujo.

- **SwapChain**  
  Administra el intercambio entre el back buffer y el front buffer, permitiendo mostrar en pantalla lo que se renderiza.

- **RenderTargetView**  
  Define dónde se dibuja la escena (el “lienzo” principal o texturas auxiliares). Se puede asociar con un DepthStencilView.

- **DepthStencilView**  
  Controla el buffer de profundidad y stencil, usado para determinar qué píxeles deben renderizarse según su distancia y reglas de stencil.

- **Texture**  
  Maneja recursos de texturas en la GPU, ya sea para imágenes cargadas o para buffers internos usados en el render.

---

## ⚙️ Requisitos

- **Windows 10/11**  
- **Visual Studio** con soporte para C++  
- **SDK de DirectX 11**  
- Compilador compatible con C++17 o superior  

---

## 🚀 Cómo compilar

1. Clonar este repositorio o descargar el proyecto.  
2. Abrir la solución en **Visual Studio**.  
3. Compilar en modo **Debug** o **Release**.  
4. Ejecutar la aplicación: se abrirá una ventana base donde se inicia el pipeline gráfico.  

---


---

## ✍️ Julio David medina Franco

Proyecto desarrollado como práctica académica para el manejo de **DirectX 11**  
con arquitectura modular en C++ y documentación Doxygen.
