#pragma once
// Librerías estándar
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>
#include <xnamath.h>
#include <thread>
#include <memory>
#include <unordered_map>
#include <type_traits>

// Librerías DirectX
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include "Resource.h"
#include "resource.h"

// Librerías de terceros (EngineUtilities: vectores y sistema de memoria)
#include "EngineUtilities/Vectors/Vector2.h"
#include "EngineUtilities/Vectors/Vector3.h"
#include "EngineUtilities\Memory\TSharedPointer.h"
#include "EngineUtilities\Memory\TWeakPointer.h"
#include "EngineUtilities\Memory\TStaticPtr.h"
#include "EngineUtilities\Memory\TUniquePtr.h"

// MACROS

/// Macro auxiliar para liberar interfaces COM de manera segura.
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;

/// Macro para mostrar mensajes de creación de recursos en la ventana de depuración.
#define MESSAGE( classObj, method, state )   \
{                                            \
   std::wostringstream os_;                  \
   os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n"; \
   OutputDebugStringW( os_.str().c_str() );  \
}

/// Macro para mostrar mensajes de error en la ventana de depuración.
/// Incluye nombre de clase, método y mensaje de error.
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

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------

/// <summary>
/// Estructura de vértice simple con posición y coordenadas de textura.
/// Usada como layout básico para modelos en este motor.
/// </summary>
struct SimpleVertex
{
  XMFLOAT3 Pos; // Posición del vértice en espacio 3D.
  XMFLOAT2 Tex; // Coordenadas de textura (UV).
};

/// <summary>
/// Constant buffer con datos que casi no cambian:
/// contiene la matriz de vista de la cámara.
/// </summary>
struct CBNeverChanges
{
  XMMATRIX mView;
};

/// <summary>
/// Constant buffer que guarda la matriz de proyección.
/// Se actualiza cuando cambia el tamaño de la ventana o el FOV.
/// </summary>
struct CBChangeOnResize
{
  XMMATRIX mProjection;
};

/// <summary>
/// Constant buffer con datos por frame:
/// matriz mundo del objeto y color de malla.
/// </summary>
struct CBChangesEveryFrame
{
  XMMATRIX mWorld;
  XMFLOAT4 vMeshColor;
};

/// <summary>
/// Extensiones soportadas para carga de texturas.
/// </summary>
enum ExtensionType {
  DDS = 0,
  PNG = 1,
  JPG = 2
};

/// <summary>
/// Tipos básicos de shader usados en la aplicación.
/// </summary>
enum ShaderType {
  VERTEX_SHADER = 0,
  PIXEL_SHADER = 1
};

/**
 * @enum ComponentType
 * @brief Tipos de componentes disponibles en el juego.
 */
enum
  ComponentType {
  NONE = 0,     ///< Tipo de componente no especificado.
  TRANSFORM = 1,///< Componente de transformación.
  MESH = 2,     ///< Componente de malla.
  MATERIAL = 3  ///< Componente de material.
};
