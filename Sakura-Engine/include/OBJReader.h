#pragma once
#include "MeshComponent.h"
#include <string>
#include <vector>
#include <unordered_map>

/*
 * Clase ObjReader
 *
 * Sirve para leer archivos .obj y llenar un MeshComponent
 * con vértices e índices.
 *
 * Lee:
 *  - v  -> posiciones
 *  - vt -> coordenadas de textura (uv)
 *  - vn -> normales
 *  - f  -> caras (polígonos)
 *
 * Las caras se convierten a triángulos usando un “fan”:
 *  (0, i, i+1)
 *
 * Usa un mapa (cache) para no repetir vértices cuando la
 * misma combinación v/vt/vn se repite.
 *
 * El parámetro flipV permite invertir la componente V de la UV
 * por si la textura está al revés verticalmente.
 */
class ObjReader {
public:
  // Constructor vacío
  ObjReader() = default;

  // Destructor por defecto
  ~ObjReader() = default;

  /*
   * Carga un archivo .obj desde 'path' y llena outMesh.
   *
   * Si 'path' ya termina en ".obj", se usa tal cual.
   * Si no, se le agrega ".obj" al final.
   *
   * flipV = true invierte la V de las coordenadas de textura.
   * Devuelve true si se cargó algo válido (tiene vértices e índices).
   */
  bool load(const std::string& path, MeshComponent& outMesh, bool flipV = true);

private:
  /*
   * Revisa si la cadena termina en ".obj" o ".OBJ".
   */
  static bool endsWithObj_(const std::string& s);

  /*
   * Parsea una tupla de índice del .obj:
   *  - "v/vt/vn"
   *  - "v/vt"
   *  - "v//vn"
   *  - "v"
   *
   * Rellena:
   *  vi = índice de posición
   *  ti = índice de texcoord
   *  ni = índice de normal
   *
   * Todos son base 0, o -1 si no existe esa parte.
   */
  static void parseTuple_(const std::string& key, int& vi, int& ti, int& ni);

  /*
   * Imprime un mensaje de advertencia en consola.
   * Útil para avisar cuando hay líneas raras en el .obj.
   */
  static void logWarn_(const std::string& msg);

  // Se deshabilita la copia del objeto para evitar duplicados
  ObjReader(const ObjReader&) = delete;
  ObjReader& operator=(const ObjReader&) = delete;
};
