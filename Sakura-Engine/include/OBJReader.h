#pragma once
#include "MeshComponent.h"
#include <string>
#include <vector>
#include <unordered_map>

/*
  ObjReader: lector simple de archivos .obj para llenar un MeshComponent.
  - Lee: v (pos), vt (uv), vn (normal), f (caras)
  - Convierte polígonos a triángulos con “fan” -> (0, i, i+1)
  - Usa un cache para no repetir vértices ("v/vt/vn")
  - flipV: opcional para invertir la coordenada V de la UV
*/
class ObjReader {
public:
  ObjReader() = default;   // ctor vacío
  ~ObjReader() = default;  // dtor por defecto

  // Carga el .obj desde 'path' y mete vértices/índices en outMesh.
  // Si 'path' ya termina en .obj, lo respeta.
  // flipV = true para invertir la V de la UV (útil según motor/textura).
  bool load(const std::string& path, MeshComponent& outMesh, bool flipV = true);

private:
  // Checa si el string termina con ".obj" (o ".OBJ").
  static bool endsWithObj_(const std::string& s);

  // Parsea una tupla tipo "v/vt/vn", "v/vt", "v//vn" o "v".
  // Devuelve índices base-0 en vi/ti/ni (o -1 si no hay).
  static void parseTuple_(const std::string& key, int& vi, int& ti, int& ni);

  // Log simple para avisos/errores (por consola).
  static void logWarn_(const std::string& msg);

  // Evita copias por accidente (no necesitamos duplicar lectores).
  ObjReader(const ObjReader&) = delete;
  ObjReader& operator=(const ObjReader&) = delete;
};
