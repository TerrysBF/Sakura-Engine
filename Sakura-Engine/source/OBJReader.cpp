#include "ObjReader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

// Reserva espacio "a ojo" para no estar realocando tanto
static inline void reserveAprox_(std::vector<XMFLOAT3>& a,
  std::vector<XMFLOAT2>& b,
  std::vector<XMFLOAT3>& c)
{
  // Si el .obj trae los conteos, sería mejor usarlos en vez de 1024
  a.reserve(1024);
  b.reserve(1024);
  c.reserve(1024);
}

// Revisa si el string termina con .obj (mayúsculas o minúsculas)
bool ObjReader::endsWithObj_(const std::string& s) {
  return s.size() >= 4 &&
    (s.compare(s.size() - 4, 4, ".obj") == 0 ||
      s.compare(s.size() - 4, 4, ".OBJ") == 0);
}

// Imprime un warning simple a la consola
void ObjReader::logWarn_(const std::string& msg) {
  // En Windows se puede usar OutputDebugStringA si quieres
  std::cerr << "[OBJ] " << msg << "\n";
}

// Parsea una tupla tipo "v/vt/vn", "v/vt", "v//vn" o "v"
void ObjReader::parseTuple_(const std::string& key, int& vi, int& ti, int& ni) {
  // vi = pos, ti = texcoord, ni = normal (aquí normal casi no se usa)
  vi = ti = ni = -1;
  std::istringstream ss(key);
  std::string part;
  if (std::getline(ss, part, '/') && !part.empty()) vi = std::stoi(part) - 1; // índice empieza en 0
  if (std::getline(ss, part, '/') && !part.empty()) ti = std::stoi(part) - 1;
  if (std::getline(ss, part, '/') && !part.empty()) ni = std::stoi(part) - 1;
}

// Carga un .obj básico (posiciones + uvs) y lo mete a MeshComponent
bool ObjReader::load(const std::string& path, MeshComponent& outMesh, bool flipV) {
  // Limpiamos lo que tenga la malla de antes
  outMesh.m_vertex.clear();
  outMesh.m_index.clear();

  // Vectores temporales para leer el archivo
  std::vector<XMFLOAT3> positions;
  std::vector<XMFLOAT2> texcoords;
  std::vector<XMFLOAT3> normals; // por si después activas normales
  reserveAprox_(positions, texcoords, normals);

  // Resultado final (va a MeshComponent)
  std::vector<SimpleVertex>   verts;   verts.reserve(2048);
  std::vector<unsigned int>   indices; indices.reserve(4096);

  // Cache para no repetir vértices: "v/vt/vn" -> índice en verts
  std::unordered_map<std::string, unsigned int> vcache;
  vcache.reserve(4096);

  // Si no viene con .obj al final, se lo agregamos
  const std::string filePath = endsWithObj_(path) ? path : (path + ".obj");
  std::ifstream ifs(filePath);
  if (!ifs.is_open()) {
    logWarn_("No se pudo abrir: " + filePath);
    return false;
  }

  std::string line;
  line.reserve(256);

  // Leemos línea por línea
  while (std::getline(ifs, line)) {
    // Saltar espacios y comentarios (líneas con #)
    size_t p = 0;
    while (p < line.size() && std::isspace(static_cast<unsigned char>(line[p]))) ++p;
    if (p >= line.size() || line[p] == '#') continue;

    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    if (tag == "v") {
      // Posición de un vértice
      XMFLOAT3 v{};
      ss >> v.x >> v.y >> v.z;
      if (!ss.fail()) positions.push_back(v);
      else logWarn_("v mal formada: " + line);
    }
    else if (tag == "vt") {
      // Coordenada de textura (u, v)
      XMFLOAT2 t{};
      ss >> t.x >> t.y;
      if (!ss.fail()) {
        // Algunas texturas vienen invertidas en V
        if (flipV) t.y = 1.0f - t.y;
        texcoords.push_back(t);
      }
      else {
        logWarn_("vt mal formada: " + line);
      }
    }
    else if (tag == "vn") {
      // Normal (de momento no la guardamos en SimpleVertex)
      float nx = 0.f, ny = 0.f, nz = 0.f;
      ss >> nx >> ny >> nz;
      // Si luego agregas normales al vertex, aquí puedes guardarlas:
      // if (!ss.fail()) normals.push_back({nx, ny, nz});
    }
    else if (tag == "f") {
      // Cara/polígono. Puede traer 3+ vértices.
      // Vamos a leer todas las tuplas y triangulizar tipo “fan”
      std::vector<std::string> tuples;
      tuples.reserve(8);
      std::string tuple;
      while (ss >> tuple) tuples.push_back(tuple);

      const unsigned int N = static_cast<unsigned int>(tuples.size());
      if (N < 3) {
        logWarn_("f con menos de 3 vértices: " + line);
        continue;
      }

      std::vector<unsigned int> local;
      local.reserve(N);

      // Para cada tupla "v/vt/vn", buscamos en cache o creamos el vértice
      for (unsigned int i = 0; i < N; ++i) {
        const std::string& key = tuples[i];

        // Si ya existe en el cache, lo usamos
        auto it = vcache.find(key);
        if (it != vcache.end()) {
          local.push_back(it->second);
          continue;
        }

        // No está en cache: parseamos y creamos SimpleVertex
        int vi, ti, ni;
        parseTuple_(key, vi, ti, ni);

        // Checamos que la posición exista
        if (vi < 0 || vi >= static_cast<int>(positions.size())) {
          logWarn_("índice v fuera de rango: " + key + "   << " + line);
          continue; // nos saltamos este vértice
        }

        SimpleVertex sv{};
        sv.Pos = positions[vi];

        // Si trae uv válido lo asignamos, si no, 0,0
        if (ti >= 0 && ti < static_cast<int>(texcoords.size()))
          sv.Tex = texcoords[ti];
        else
          sv.Tex = XMFLOAT2(0.f, 0.f);

        // Metemos el vértice nuevo y guardamos su índice
        verts.push_back(sv);
        const unsigned int newIdx = static_cast<unsigned int>(verts.size() - 1);
        vcache.emplace(key, newIdx);
        local.push_back(newIdx);
      }

      // Si por errores quedó menos de 3, no triangulamos
      if (local.size() < 3) continue;

      // Triangulación tipo “fan”: (0, i, i+1)
      for (unsigned int i = 1; i + 1 < local.size(); ++i) {
        indices.push_back(local[0]);
        indices.push_back(local[i]);
        indices.push_back(local[i + 1]);
      }
    }
    // Por ahora ignoramos otros tags: g/o/usemtl/mtllib (se pueden agregar después)
  }

  ifs.close();

  // Pasamos el resultado al MeshComponent
  outMesh.m_vertex = std::move(verts);
  outMesh.m_index = std::move(indices);
  outMesh.m_numVertex = static_cast<int>(outMesh.m_vertex.size());
  outMesh.m_numIndex = static_cast<int>(outMesh.m_index.size());

  // Regresamos true si sí cargó algo
  return (outMesh.m_numVertex > 0 && outMesh.m_numIndex > 0);
}
