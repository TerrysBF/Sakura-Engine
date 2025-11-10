#pragma once

// Librerías de la STL
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>
#include <xnamath.h>
#include <thread>

// Librerías de DirectX
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include "Resource.h"
#include "resource.h"

// Aquí podrían ir librerías de terceros (por ahora nada)

/*
 * Macro para liberar un recurso COM de forma segura.
 * Si el puntero no es nullptr, llama a Release() y luego lo pone en nullptr.
 */
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;

 /*
  * Macro para escribir mensajes en el debug output.
  * Sirve para avisar cuando se crea algún recurso (texturas, buffers, etc.).
  *
  * classObj -> nombre de la clase (texto)
  * method   -> nombre del método (texto)
  * state    -> mensaje que queremos mostrar
  */
#define MESSAGE( classObj, method, state )   \
{                                            \
   std::wostringstream os_;                  \
   os_ << classObj << "::" << method << " : " \
       << "[CREATION OF RESOURCE : " << state << "] \n"; \
   OutputDebugStringW( os_.str().c_str() );  \
}

  /*
   * Macro para escribir errores en el debug output.
   * Se usa cuando algo falla (por ejemplo, al crear un buffer o textura).
   *
   * classObj -> nombre de la clase (texto)
   * method   -> nombre del método (texto)
   * errorMSG -> texto del error
   */
#define ERROR(classObj, method, errorMSG)                     \
{                                                             \
    try {                                                     \
        std::wostringstream os_;                              \
        os_ << L"ERROR : " << classObj << L"::" << method     \
            << L" : " << errorMSG << L"\n";                   \
        OutputDebugStringW(os_.str().c_str());                \
    } catch (...) {                                           \
        OutputDebugStringW(L"Failed to log error message.\n");\
    }                                                         \
}

   // Estructura de vértice simple: posición y UV
struct SimpleVertex
{
  XMFLOAT3 Pos; // posición 3D del vértice (x, y, z)
  XMFLOAT2 Tex; // coordenadas de textura (u, v)
};

// Constant buffer para la matriz de vista (normalmente no cambia mucho)
struct CBNeverChanges
{
  XMMATRIX mView; // matriz de vista (cámara)
};

// Constant buffer para la matriz de proyección (se actualiza al cambiar el tamaño de la ventana)
struct CBChangeOnResize
{
  XMMATRIX mProjection; // matriz de proyección (perspectiva)
};

// Constant buffer para datos que cambian cada frame
struct CBChangesEveryFrame
{
  XMMATRIX mWorld;    // matriz mundo (transformación del objeto)
  XMFLOAT4 vMeshColor; // color del mesh (RGBA)
};

// Tipo de extensión de textura soportada por el sistema
enum ExtensionType {
  DDS = 0, // texturas .dds
  PNG = 1, // texturas .png
  JPG = 2  // texturas .jpg
};

// Tipo de shader que se quiere crear/usar
enum ShaderType {
  VERTEX_SHADER = 0, // shader de vértices
  PIXEL_SHADER = 1  // shader de píxeles
};
