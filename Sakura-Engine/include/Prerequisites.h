#pragma once
// Librerias STD
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>
#include <xnamath.h>
#include <thread>

// Librerias DirectX
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include "Resource.h"
#include "resource.h"

// Third Party Libraries

/**
 * @brief Libera un recurso COM de forma segura.
 * @details Si el puntero no es nullptr, llama Release() y lo pone en nullptr.
 */
#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x = nullptr;

 /**
  * @brief Macro para loguear mensajes de creación de recursos.
  * @param classObj nombre de la clase (texto)
  * @param method   nombre del método (texto)
  * @param state    estado/resultado a mostrar
  */
#define MESSAGE( classObj, method, state )   \
{                                            \
   std::wostringstream os_;                  \
   os_ << classObj << "::" << method << " : " << "[CREATION OF RESOURCE " << ": " << state << "] \n"; \
   OutputDebugStringW( os_.str().c_str() );  \
}

  /**
   * @brief Macro para loguear errores en OutputDebugStringW.
   * @param classObj nombre de la clase (texto)
   * @param method   nombre del método (texto)
   * @param errorMSG texto del error
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

   //--------------------------------------------------------------------------------------
   // Structures
   //--------------------------------------------------------------------------------------

   /**
    * @struct SimpleVertex
    * @brief Vértice básico con posición y UV.
    * @var SimpleVertex::Pos Posición 3D.
    * @var SimpleVertex::Tex Coordenadas de textura (u,v).
    */
struct SimpleVertex
{
  XMFLOAT3 Pos;
  XMFLOAT2 Tex;
};

/**
 * @struct CBNeverChanges
 * @brief Constant buffer para datos que casi no cambian.
 * @var CBNeverChanges::mView Matriz de vista.
 */
struct CBNeverChanges
{
  XMMATRIX mView;
};

/**
 * @struct CBChangeOnResize
 * @brief Constant buffer para datos al redimensionar.
 * @var CBChangeOnResize::mProjection Matriz de proyección.
 */
struct CBChangeOnResize
{
  XMMATRIX mProjection;
};

/**
 * @struct CBChangesEveryFrame
 * @brief Constant buffer para datos por frame.
 * @var CBChangesEveryFrame::mWorld     Matriz mundo.
 * @var CBChangesEveryFrame::vMeshColor Color del mesh (RGBA).
 */
struct CBChangesEveryFrame
{
  XMMATRIX mWorld;
  XMFLOAT4 vMeshColor;
};

/**
 * @enum ExtensionType
 * @brief Tipos simples de textura soportados.
 */
enum ExtensionType {
  DDS = 0,
  PNG = 1,
  JPG = 2
};
