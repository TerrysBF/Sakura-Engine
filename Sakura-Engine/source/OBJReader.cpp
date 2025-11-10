#include "ObjReader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

// Función auxiliar para reservar memoria aproximada en los vectores.
// La idea es evitar muchas realocaciones mientras se lee el .obj.
static inline void reserveAprox_(std::vector<XMFLOAT3>& a,
  std::vector<XMFLOAT2>& b,
  std::vector<XMFLOAT3>& c)
{
  // Aquí solo reservo un tamaño fijo "grande" por defecto.
  // Si el .obj tuviera el conteo real, sería mejor usarlo.
  a.reserve(1024);
  b.reserve(1024);
  c.reserve(1024);
}

// Revisa si el string termina con ".obj" o ".OBJ".
// Esto sirve para saber si hay que agregar la extensión o no.
bool ObjReader::endsWithObj_(const std::string& s) {
  return s.size() >= 4 &&
    (s.compare(s.size() - 4, 4, ".obj") == 0 ||
      s.compare(s.size() - 4, 4, ".OBJ") == 0);
}

// Muestra un mensaje de advertencia sencillo por consola.
// Se usa para informar de líneas mal formadas o problemas de lectura.
void ObjReader::logWarn_(const std::string& msg) {
  // También se podría usar OutputDebugStringA en Windows.
  std::cerr << "[OBJ] " << msg << "\n";
}

// Parsea una tupla de índice del .obj, por ejemplo "v/vt/vn", "v/vt", "v//vn" o "v".
// Devuelve los índices de posición, texcoord y normal como enteros base 0.
void ObjReader::parseTuple_(const std::string& key, int& vi, int& ti, int& ni) {
  // vi = índice de posición, ti = índice de UV, ni = índice de normal.
  // Aquí inicializo todo a -1 para indicar "no tiene".
  vi = ti = ni = -1;

  // Uso un stringstream para ir separando por el carácter '/'.
  std::istringstream ss(key);
  std::string part;

  // Primer campo: posición (v). En .obj empieza en 1, así que resto 1.
  if (std::getline(ss, part, '/') && !part.empty()) vi = std::stoi(part) - 1;

  // Segundo campo: texcoord (vt).
  if (std::getline(ss, part, '/') && !part.empty()) ti = std::stoi(part) - 1;

  // Tercer campo: normal (vn).
  if (std::getline(ss, part, '/') && !part.empty()) ni = std::stoi(part) - 1;
}

// Carga un archivo .obj sencillo (posiciones y UVs) y rellena un MeshComponent.
// flipV sirve para invertir la coordenada V de las UV si hace falta.
bool ObjReader::load(const std::string& path, MeshComponent& outMesh, bool flipV) {
  // Limpio primero los datos que hubiera en la malla de salida.
  outMesh.m_vertex.clear();
  outMesh.m_index.clear();

  // Vectores temporales para guardar lo que viene del archivo.
  // positions: lista de "v"
  // texcoords: lista de "vt"
  // normals: lista de "vn" (por ahora casi no se usa)
  std::vector<XMFLOAT3> positions;
  std::vector<XMFLOAT2> texcoords;
  std::vector<XMFLOAT3> normals;
  reserveAprox_(positions, texcoords, normals);

  // Estos vectores son el resultado final que se copiará al MeshComponent.
  std::vector<SimpleVertex>   verts;
  verts.reserve(2048);            // reservo algo de espacio extra
  std::vector<unsigned int>   indices;
  indices.reserve(4096);

  // Cache para no crear vértices duplicados.
  // La clave es la tupla completa en texto, por ejemplo "1/2/3".
  // El valor es el índice del vértice en el vector verts.
  std::unordered_map<std::string, unsigned int> vcache;
  vcache.reserve(4096);

  // Si la ruta no trae ".obj" al final, se lo agrego.
  const std::string filePath = endsWithObj_(path) ? path : (path + ".obj");

  // Abro el archivo .obj.
  std::ifstream ifs(filePath);
  if (!ifs.is_open()) {
    logWarn_("No se pudo abrir: " + filePath);
    return false;
  }

  std::string line;
  line.reserve(256);

  // Leo el archivo línea por línea.
  while (std::getline(ifs, line)) {
    // Avanzo los espacios en blanco del inicio.
    size_t p = 0;
    while (p < line.size() && std::isspace(static_cast<unsigned char>(line[p]))) ++p;

    // Si la línea está vacía o empieza con '#', la salto.
    if (p >= line.size() || line[p] == '#') continue;

    // Uso un stringstream para leer el "tag" de la línea, por ejemplo "v", "vt", "vn" o "f".
    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    if (tag == "v") {
      // Línea de vértice: posición en 3D.
      XMFLOAT3 v{};
      ss >> v.x >> v.y >> v.z;
      if (!ss.fail())
        positions.push_back(v);
      else
        logWarn_("v mal formada: " + line);
    }
    else if (tag == "vt") {
      // Línea de coordenada de textura (u, v).
      XMFLOAT2 t{};
      ss >> t.x >> t.y;
      if (!ss.fail()) {
        // A veces la V viene al revés, así que se puede invertir si flipV es true.
        if (flipV) t.y = 1.0f - t.y;
        texcoords.push_back(t);
      }
      else {
        logWarn_("vt mal formada: " + line);
      }
    }
    else if (tag == "vn") {
      // Línea de normal 3D.
      // De momento no la usamos en SimpleVertex, pero la leo por si después se quiere usar.
      float nx = 0.f, ny = 0.f, nz = 0.f;
      ss >> nx >> ny >> nz;
      // Aquí se podría guardar en normals si se necesitara.
      // if (!ss.fail()) normals.push_back({nx, ny, nz});
    }
    else if (tag == "f") {
      // Línea de cara (face). Puede tener 3 o más vértices.
      // Primero guardo todas las tuplas como strings: "v/vt/vn".
      std::vector<std::string> tuples;
      tuples.reserve(8);
      std::string tuple;
      while (ss >> tuple) tuples.push_back(tuple);

      const unsigned int N = static_cast<unsigned int>(tuples.size());
      if (N < 3) {
        // Si hay menos de 3 vértices no se puede formar un triángulo.
        logWarn_("f con menos de 3 vértices: " + line);
        continue;
      }

      // En este vector guardo los índices de vértice que corresponden a esta cara.
      std::vector<unsigned int> local;
      local.reserve(N);

      // Recorro cada tupla "v/vt/vn".
      for (unsigned int i = 0; i < N; ++i) {
        const std::string& key = tuples[i];

        // Reviso si esta combinación ya existe en el cache.
        auto it = vcache.find(key);
        if (it != vcache.end()) {
          // Si ya existe, solo uso el índice que ya tengo.
          local.push_back(it->second);
          continue;
        }

        // Si no está en cache, parseo la tupla para obtener los índices numéricos.
        int vi, ti, ni;
        parseTuple_(key, vi, ti, ni);

        // Verifico que el índice de posición sea válido.
        if (vi < 0 || vi >= static_cast<int>(positions.size())) {
          logWarn_("índice v fuera de rango: " + key + "   << " + line);
          continue; // salto este vértice y sigo con el siguiente.
        }

        // Armo un SimpleVertex con la posición y la UV.
        SimpleVertex sv{};
        sv.Pos = positions[vi];

        // Si hay un índice de texcoord válido, lo uso; si no, pongo (0,0).
        if (ti >= 0 && ti < static_cast<int>(texcoords.size()))
          sv.Tex = texcoords[ti];
        else
          sv.Tex = XMFLOAT2(0.f, 0.f);

        // Agrego el nuevo vértice al vector de vértices finales.
        verts.push_back(sv);

        // El índice del nuevo vértice es el último del vector.
        const unsigned int newIdx = static_cast<unsigned int>(verts.size() - 1);

        // Lo guardo en el cache para futuras repeticiones de la misma tupla.
        vcache.emplace(key, newIdx);

        // También lo guardo en la lista local de esta cara.
        local.push_back(newIdx);
      }

      // Si por algún error quedaron menos de 3 vértices válidos, no se triangula.
      if (local.size() < 3) continue;

      // Convierto la cara con N vértices en triángulos usando modo "fan":
      // (0, 1, 2), (0, 2, 3), etc.
      for (unsigned int i = 1; i + 1 < local.size(); ++i) {
        indices.push_back(local[0]);
        indices.push_back(local[i]);
        indices.push_back(local[i + 1]);
      }
    }
    // Aquí se podrían manejar otros tags del .obj como g, o, usemtl, mtllib, etc.
  }

  // Cierro el archivo .obj.
  ifs.close();

  // Copio los datos finales a la malla de salida.
  outMesh.m_vertex = std::move(verts);
  outMesh.m_index = std::move(indices);
  outMesh.m_numVertex = static_cast<int>(outMesh.m_vertex.size());
  outMesh.m_numIndex = static_cast<int>(outMesh.m_index.size());

  // Regreso true si se cargó al menos un vértice y un índice.
  return (outMesh.m_numVertex > 0 && outMesh.m_numIndex > 0);
}
