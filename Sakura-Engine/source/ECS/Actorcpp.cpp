#include "ECS/Actor.h"
#include "ECS/Transform.h"
#include "MeshComponent.h"
#include "Device.h"
#include "DeviceContext.h"

Actor::Actor(Device& device) {
  // --- Componentes por defecto: Transform + (opcionalmente MeshComponent) ---

  // Transform como componente principal de transformación
  Transform* transform = new Transform();
  addComponent(transform);

  // Si en el futuro quieres un MeshComponent como componente ECS,
  // puedes descomentar esto y usarlo:
  // MeshComponent* meshComponent = new MeshComponent();
  // addComponent(meshComponent);

  HRESULT hr;
  std::string classNameType = "Actor -> " + m_name;

  // Constant buffer del modelo (igual tamaño que CBChangesEveryFrame)
  hr = m_modelBuffer.init(device, sizeof(CBChangesEveryFrame));
  if (FAILED(hr)) {
    ERROR("Actor", classNameType.c_str(),
      "Failed to create new CBChangesEveryFrame");
  }

  // Sampler para las texturas
  hr = m_sampler.init(device);
  if (FAILED(hr)) {
    ERROR("Actor", classNameType.c_str(),
      "Failed to create new SamplerState");
  }

  // Posición de luz (por si después usas sombras)
  m_LightPos = XMFLOAT4(2.0f, 4.0f, -2.0f, 1.0f);
}

void
Actor::update(float deltaTime, DeviceContext& deviceContext) {
  // Actualizar todos los componentes (Transform, etc.)
  for (Component* component : m_components) {
    if (component) {
      component->update(deltaTime);
    }
  }

  // Obtener el Transform del actor (si existe)
  Transform* transform = getComponent<Transform>();
  if (transform) {
    // Subimos la world matrix al constant buffer del modelo
    m_model.mWorld = XMMatrixTranspose(transform->matrix);
    m_model.vMeshColor = XMFLOAT4(1, 1, 1, 1); // color blanco por defecto

    m_modelBuffer.update(deviceContext, nullptr, 0, nullptr, &m_model, 0, 0);
  }
}

void
Actor::render(DeviceContext& deviceContext) {
  // Sampler para el pixel shader
  m_sampler.render(deviceContext, 0, 1);

  deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // Dibujar todas las mallas asociadas al actor
  for (size_t i = 0; i < m_meshes.size(); ++i) {
    // Vertex / Index buffers
    m_vertexBuffers[i].render(deviceContext, 0, 1);
    m_indexBuffers[i].render(deviceContext, 0, 1, false, DXGI_FORMAT_R32_UINT);

    // Constant buffer del modelo (VS + PS)
    m_modelBuffer.render(deviceContext, 1, 1, true);

    // Texturas (por ahora usamos solo la 0 como albedo)
    if (!m_textures.empty()) {
      size_t texIndex = (i < m_textures.size()) ? i : 0;
      m_textures[texIndex].render(deviceContext, 0, 1);
    }

    deviceContext.DrawIndexed(m_meshes[i].m_numIndex, 0, 0);
  }
}

void
Actor::destroy() {
  // Buffers de malla
  for (auto& vb : m_vertexBuffers) {
    vb.destroy();
  }

  for (auto& ib : m_indexBuffers) {
    ib.destroy();
  }

  // Texturas
  for (auto& tex : m_textures) {
    tex.destroy();
  }

  // Constant buffer y sampler
  m_modelBuffer.destroy();
  m_sampler.destroy();

  // Destruir componentes del ECS
  for (Component* c : m_components) {
    if (c) {
      c->destroy();   // por si el componente reserva recursos internos
      delete c;
    }
  }
  m_components.clear();
}

void
Actor::setMesh(Device& device, std::vector<MeshComponent> meshes) {
  m_meshes = std::move(meshes);

  m_vertexBuffers.clear();
  m_indexBuffers.clear();

  m_vertexBuffers.resize(m_meshes.size());
  m_indexBuffers.resize(m_meshes.size());

  HRESULT hr;

  for (size_t i = 0; i < m_meshes.size(); ++i) {
    const MeshComponent& mc = m_meshes[i];

    // Vertex buffer
    hr = m_vertexBuffers[i].init(device, mc, D3D11_BIND_VERTEX_BUFFER);
    if (FAILED(hr)) {
      ERROR("Actor", "setMesh",
        ("Failed to create VertexBuffer for mesh " + std::to_string(i)).c_str());
    }

    // Index buffer
    hr = m_indexBuffers[i].init(device, mc, D3D11_BIND_INDEX_BUFFER);
    if (FAILED(hr)) {
      ERROR("Actor", "setMesh",
        ("Failed to create IndexBuffer for mesh " + std::to_string(i)).c_str());
    }
  }
}

void
Actor::renderShadow(DeviceContext& /*deviceContext*/) {
  // Stub vacío por ahora.
  // Aquí iría el pipeline de shadow mapping si lo implementas después.
}
