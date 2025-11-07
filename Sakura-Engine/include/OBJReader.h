#pragma once
#pragma once
#include "MeshComponent.h"
#include <string>
#include <vector>
#include <unordered_map>

/*
  ObjReader: lector sencillo de .obj para volcar geometría en MeshComponent.
  - Soporta v / vt / vn / f
  - Triangula n-gons en fan (0, i, i+1)
  - Cachea vértices por tupla textual ("v/vt/vn")
  - Flip de V opcional (por defecto activado)
*/
class ObjReader {
public:
  ObjReader() = default;
  ~ObjReader() = default;

  // Nota: si 'path' ya trae .obj, se respeta.
  bool load(const std::string& path, MeshComponent& outMesh, bool flipV = true);

private:
  static bool endsWithObj_(const std::string& s);
  static void parseTuple_(const std::string& key, int& vi, int& ti, int& ni);

  // Pequeña ayuda para logs; no me voy a poner fancy.
  static void logWarn_(const std::string& msg);

  // Evitar copias accidentales
  ObjReader(const ObjReader&) = delete;
  ObjReader& operator=(const ObjReader&) = delete;
};
