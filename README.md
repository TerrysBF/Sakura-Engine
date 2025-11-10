# Sakura-Engine

Este es un compendio de las clases vistas en las materia de Graficas Computacionales 3D, Generación 2026 -01

# Wildvine Engine – Carga de modelo OBJ con DirectX 11

Proyecto de DirectX 11 en C++ que muestra un modelo 3D (`Alien.obj`) cargado desde un archivo OBJ, texturizado y renderizado en una ventana Win32.  
Incluye un parser sencillo de OBJ (`ObjReader`) y una mini “engine” con clases como `Device`, `SwapChain`, `ShaderProgram`, `MeshComponent`, etc.

---

## Requisitos

- Windows 10/11
- Visual Studio (2019 o 2022 recomendado)
- Windows SDK con DirectX 11
- Librerías de DirectX:
  - `d3d11.h`
  - `d3dx11.h`
  - `d3dcompiler.h`

> Nota: `D3DX11` es una librería antigua. Asegúrate de tenerla añadida a las dependencias del proyecto (Additional Include/Library Directories y Additional Dependencies).

---

## Estructura general del proyecto

Componentes principales:

- `BaseApp`  
  Se encarga del ciclo principal:
  - Crear ventana (`Window`)
  - Inicializar D3D (`Device`, `DeviceContext`, `SwapChain`)
  - Crear RTV / DSV / Viewport
  - Cargar shaders (`ShaderProgram`)
  - Cargar modelo (`ObjReader` → `MeshComponent`)
  - Crear buffers (`Buffer`)
  - Cargar textura (`Texture`) y sampler (`SamplerState`)
  - Bucle `update()` / `render()`

- `ObjReader`  
  Parser simple de archivos `.obj`.  
  Lee:
  - `v`  (posición)
  - `vt` (coordenadas de textura)
  - `vn` (normales, de momento casi no se usan)
  - `f`  (caras, triangulizadas por “fan” → (0, i, i+1))  
  Llena un `MeshComponent` con:
  - `m_vertex` (lista de `SimpleVertex`)
  - `m_index`  (índices de triángulos)

- `MeshComponent`  
  Estructura que guarda:
  - Nombre de la malla
  - Vértices (`std::vector<SimpleVertex>`)
  - Índices (`std::vector<unsigned int>`)
  - Contadores `m_numVertex`, `m_numIndex`

- `Buffer`  
  Wrapper de `ID3D11Buffer`:
  - Crea Vertex Buffer / Index Buffer a partir de `MeshComponent`
  - Crea Constant Buffers
  - Hace `UpdateSubresource` y los bindea al pipeline

- `ShaderProgram`  
  Maneja:
  - Vertex Shader
  - Pixel Shader
  - Input Layout (posición + UV)
  Compila el archivo HLSL (`Sakura-Engine.fx`) y setea los shaders en el pipeline.

- `Texture` + `SamplerState`  
  - `Texture::init(...)` carga una textura desde archivo (`PNG/JPG/DDS`) con `D3DX11CreateShaderResourceViewFromFile`.
  - `SamplerState` crea un sampler linear con wrap en U/V/W y lo bindea al pixel shader.

- `Window`, `SwapChain`, `RenderTargetView`, `DepthStencilView`, `Viewport`  
  Clases de apoyo para:
  - Crear la ventana Win32
  - Crear el swap chain con MSAA
  - Crear la Render Target View y Depth Stencil View
  - Configurar el viewport

---

## Assets necesarios

Coloca estos archivos en el **directorio de trabajo del ejecutable**  
(por defecto, suele ser la carpeta `$(TargetDir)` del proyecto, por ejemplo `Debug/` o `Release/`):

- `Alien.obj`
- `Alien.mtl`  (opcional, por ahora el parser no usa materiales pero se verifica su existencia)
- `Alien_Texture.png`  (o el formato que uses, en el código se usa `Alien_Texture` + `.png`)

En el código (`BaseApp.cpp`) se usan:

```cpp
static const wchar_t* OBJ_NAME  = L"Alien.obj";
static const wchar_t* MTL_NAME  = L"Alien.mtl";
static const char*   TEX_BASE   = "Alien_Texture"; // sin extensión
static const char*   FX_NAME    = "Sakura-Engine.fx";
