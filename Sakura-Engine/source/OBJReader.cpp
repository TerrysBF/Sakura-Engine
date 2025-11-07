#include "ObjReader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

static inline void reserveAprox_(std::vector<XMFLOAT3>& a,
  std::vector<XMFLOAT2>& b,
  std::vector<XMFLOAT3>& c)
{
  // TODO: si el .obj trae conteos a tope, usar esos en lugar de estos “a ojo”.
  a.reserve(1024);
  b.reserve(1024);
  c.reserve(1024);
}

bool ObjReader::endsWithObj_(const std::string& s) {
  return s.size() >= 4 &&
    (s.compare(s.size() - 4, 4, ".obj") == 0 ||
      s.compare(s.size() - 4, 4, ".OBJ") == 0);
}

void ObjReader::logWarn_(const std::string& msg) {
  // Si estás en Win32, cámbialo por OutputDebugStringA si te acomoda.
  std::cerr << "[OBJ] " << msg << "\n";
}

void ObjReader::parseTuple_(const std::string& key, int& vi, int& ti, int& ni) {
  // key: "v/vt/vn" | "v//vn" | "v/vt" | "v"
  vi = ti = ni = -1;
  std::istringstream ss(key);
  std::string part;
  if (std::getline(ss, part, '/') && !part.empty()) vi = std::stoi(part) - 1;
  if (std::getline(ss, part, '/') && !part.empty()) ti = std::stoi(part) - 1;
  if (std::getline(ss, part, '/') && !part.empty()) ni = std::stoi(part) - 1;
}

bool ObjReader::load(const std::string& path, MeshComponent& outMesh, bool flipV) {
  outMesh.m_vertex.clear();
  outMesh.m_index.clear();

  // Acumuladores temporales del .obj
  std::vector<XMFLOAT3> positions;
  std::vector<XMFLOAT2> texcoords;
  std::vector<XMFLOAT3> normals; // seguimos reservando por si luego activas normales
  reserveAprox_(positions, texcoords, normals);

  // Resultado final (lo que va a MeshComponent)
  std::vector<SimpleVertex>   verts;   verts.reserve(2048);
  std::vector<unsigned int>   indices; indices.reserve(4096);

  // Cache textual: "v/vt/vn" -> índice final
  std::unordered_map<std::string, unsigned int> vcache;
  vcache.reserve(4096);

  // Respeta si ya viene .obj
  const std::string filePath = endsWithObj_(path) ? path : (path + ".obj");
  std::ifstream ifs(filePath);
  if (!ifs.is_open()) {
    logWarn_("No se pudo abrir: " + filePath);
    return false;
  }

  std::string line;
  line.reserve(256);

  while (std::getline(ifs, line)) {
    // Saltar espacios y comentarios rápidos
    size_t p = 0;
    while (p < line.size() && std::isspace(static_cast<unsigned char>(line[p]))) ++p;
    if (p >= line.size() || line[p] == '#') continue;

    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    if (tag == "v") {
      XMFLOAT3 v{};
      ss >> v.x >> v.y >> v.z;
      if (!ss.fail()) positions.push_back(v);
      else logWarn_("v mal formada: " + line);
    }
    else if (tag == "vt") {
      XMFLOAT2 t{};
      ss >> t.x >> t.y;
      if (!ss.fail()) {
        if (flipV) t.y = 1.0f - t.y;
        texcoords.push_back(t);
      }
      else {
        logWarn_("vt mal formada: " + line);
      }
    }
    else if (tag == "vn") {
      // SimpleVertex no tiene 'Normal', así que solo consumimos los valores y seguimos.
      float nx = 0.f, ny = 0.f, nz = 0.f;
      ss >> nx >> ny >> nz; // opcional: podrías almacenar en 'normals' si luego agregas normales
      // if (!ss.fail()) normals.push_back({nx, ny, nz}); // activar cuando uses normales
    }
    else if (tag == "f") {
      // Leer N tuplas y triangulizar en fan
      std::vector<std::string> tuples;
      tuples.reserve(8);
      std::string tuple;
      while (ss >> tuple) tuples.push_back(tuple);

      const unsigned int N = static_cast<unsigned int>(tuples.size());
      if (N < 3) {
        logWarn_("f con menos de 3 vértices: " + line);
        continue;
      }

      std::vector<unsigned int> local; local.reserve(N);

      // Mapear/crear cada vértice de la cara
      for (unsigned int i = 0; i < N; ++i) {
        const std::string& key = tuples[i];

        // cache hit
        auto it = vcache.find(key);
        if (it != vcache.end()) {
          local.push_back(it->second);
          continue;
        }

        // cache miss ? parsear y construir SimpleVertex
        int vi, ti, ni;
        parseTuple_(key, vi, ti, ni);

        if (vi < 0 || vi >= static_cast<int>(positions.size())) {
          logWarn_("índice v fuera de rango: " + key + "   << " + line);
          continue; // saltamos este vértice
        }

        SimpleVertex sv{};
        sv.Pos = positions[vi];

        if (ti >= 0 && ti < static_cast<int>(texcoords.size()))
          sv.Tex = texcoords[ti];
        else
          sv.Tex = XMFLOAT2(0.f, 0.f); // default simple

        verts.push_back(sv);
        const unsigned int newIdx = static_cast<unsigned int>(verts.size() - 1);
        vcache.emplace(key, newIdx);
        local.push_back(newIdx);
      }

      // Si por rangos descartamos alguno, puede quedar <3
      if (local.size() < 3) continue;

      // Triangulación fan: (0, i, i+1)
      for (unsigned int i = 1; i + 1 < local.size(); ++i) {
        indices.push_back(local[0]);
        indices.push_back(local[i]);
        indices.push_back(local[i + 1]);
      }
    }
    // Nota: por ahora ignoro g/o/usemtl/mtllib. Si lo necesitas, lo sumamos luego.
  }

  ifs.close();

  outMesh.m_vertex = std::move(verts);
  outMesh.m_index = std::move(indices);
  outMesh.m_numVertex = static_cast<int>(outMesh.m_vertex.size());
  outMesh.m_numIndex = static_cast<int>(outMesh.m_index.size());

  // “Sanity check” mínimo
  return (outMesh.m_numVertex > 0 && outMesh.m_numIndex > 0);
}
