#include "ECS/Actor.h"
#include "MeshComponent.h"
#include "Device.h"
#include "DeviceContext.h"

/// <summary>
/// Constructor del Actor.
/// Crea componentes por defecto (Transform y MeshComponent) e inicializa
/// los recursos necesarios para el modelo (constant buffer y sampler).
/// </summary>
/// <param name="device">Dispositivo usado para crear los recursos gráficos.</param>
Actor::Actor(Device& device) {
	// Setup Default Components
	EU::TSharedPointer<Transform> transform = EU::MakeShared<Transform>();
	addComponent(transform);
	EU::TSharedPointer<MeshComponent> meshComponent = EU::MakeShared<MeshComponent>();
	addComponent(meshComponent);

	HRESULT hr;
	std::string classNameType = "Actor -> " + m_name;

	// Constant buffer para la información de modelo (mundo, color, etc.)
	hr = m_modelBuffer.init(device, sizeof(CBChangesEveryFrame));
	if (FAILED(hr)) {
		ERROR("Actor", classNameType.c_str(), "Failed to create new CBChangesEveryFrame");
	}

	// Sampler para las texturas del actor
	hr = m_sampler.init(device);
	if (FAILED(hr)) {
		ERROR("Actor", classNameType.c_str(), "Failed to create new SamplerState");
	}

	// Estados adicionales comentados por ahora
	//hr = m_rasterizer.init(device);
	//if (FAILED(hr)) {
	//	ERROR("Actor", classNameType.c_str(), "Failed to create new Rasterizer");
	//}

	//hr = m_blendstate.init(device);
	//if (FAILED(hr)) {
	//	ERROR("Actor", classNameType.c_str(), "Failed to create new BlendState");
	//}

	// Shader y buffers para sombras (actualmente no usados)
	//hr = m_shaderShadow.CreateShader(device, PIXEL_SHADER, "HybridEngine.fx");
	//if (FAILED(hr)) {
	//	ERROR("Main", "InitDevice",
	//		("Failed to initialize Shadow Shader. HRESULT: " + std::to_string(hr)).c_str());
	//}
	//
	//hr = m_shaderBuffer.init(device, sizeof(CBChangesEveryFrame));
	//if (FAILED(hr)) {
	//	ERROR("Main", "InitDevice",
	//		("Failed to initialize Shadow Buffer. HRESULT: " + std::to_string(hr)).c_str());
	//}
	//
	//hr = m_shadowBlendState.init(device);
	//if (FAILED(hr)) {
	//	ERROR("Main", "InitDevice",
	//		("Failed to initialize Shadow Blend State. HRESULT: " + std::to_string(hr)).c_str());
	//}
	//
	//hr = m_shadowDepthStencilState.init(device, true, false);
	//if (FAILED(hr)) {
	//	ERROR("Main", "InitDevice",
	//		("Failed to initialize Depth Stencil State. HRESULT: " + std::to_string(hr)).c_str());
	//}
	//
	//m_LightPos = XMFLOAT4(2.0f, 4.0f, -2.0f, 1.0f);
}

/// <summary>
/// Actualiza el estado del actor en cada frame.
/// Llama a update() en todos los componentes y actualiza el constant buffer del modelo.
/// </summary>
/// <param name="deltaTime">Tiempo transcurrido desde el último frame.</param>
/// <param name="deviceContext">Contexto de dispositivo para actualizar buffers.</param>
void
Actor::update(float deltaTime, DeviceContext& deviceContext) {
	// Actualizar todos los componentes
	for (auto& component : m_components) {
		if (component) {
			component->update(deltaTime);
		}
	}

	// Actualizar los datos del modelo (matriz mundo y color del mesh)
	m_model.mWorld = XMMatrixTranspose(getComponent<Transform>()->matrix);
	m_model.vMeshColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Actualizar el constant buffer asociado al modelo
	m_modelBuffer.update(deviceContext, nullptr, 0, nullptr, &m_model, 0, 0);
}

/// <summary>
/// Renderiza el actor: configura el sampler, los buffers de malla,
/// el constant buffer del modelo y las texturas necesarias.
/// </summary>
/// <param name="deviceContext">Contexto de dispositivo usado para dibujar.</param>
void
Actor::render(DeviceContext& deviceContext) {
	// 1) Proyectar sombra primero (sobre el suelo)
	//if (canCastShadow()) {
	//	renderShadow(deviceContext);
	//}
	//
	// 2) Estados de raster, blend y sampler para el modelo
	//m_blendstate.render(deviceContext);
	//m_rasterizer.render(deviceContext);

	// Asignar sampler para las texturas del actor
	m_sampler.render(deviceContext, 0, 1);

	// Topología de triángulos para dibujar las mallas
	deviceContext.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Actualizar buffer y dibujar todas las mallas del actor
	for (unsigned int i = 0; i < m_meshes.size(); i++) {
		// Asignar vertex e index buffer de la malla actual
		m_vertexBuffers[i].render(deviceContext, 0, 1);
		m_indexBuffers[i].render(deviceContext, 0, 1, false, DXGI_FORMAT_R32_UINT);

		// Bind del constant buffer del modelo (world + color)
		m_modelBuffer.render(deviceContext, 2, 1, true);

		// Render de texturas (al menos el albedo)
		if (m_textures.size() > 0) {
			if (i < m_textures.size()) {
				if (m_textures.size() >= 1) {
					m_textures[0].render(deviceContext, 0, 1); // Albedo -> t0
					//m_textures[1].render(deviceContext, 1, 1); // Normal -> t1
					//m_textures[2].render(deviceContext, 2, 1); // Metallic -> t2
					//m_textures[3].render(deviceContext, 3, 1); // Roughness -> t3
					//m_textures[4].render(deviceContext, 4, 1); // AO -> t4
				}
			}
		}

		// Dibujar la malla actual
		deviceContext.DrawIndexed(m_meshes[i].m_numIndex, 0, 0);
	}
}

/// <summary>
/// Libera los recursos del actor asociados a buffers, texturas
/// y estados gráficos básicos.
/// </summary>
void
Actor::destroy() {
	// Liberar vertex buffers
	for (auto& vertexBuffer : m_vertexBuffers) {
		vertexBuffer.destroy();
	}

	// Liberar index buffers
	for (auto& indexBuffer : m_indexBuffers) {
		indexBuffer.destroy();
	}

	// Liberar texturas asociadas al actor
	for (auto& tex : m_textures) {
		tex.destroy();
	}

	// Liberar constant buffer del modelo
	m_modelBuffer.destroy();

	// Estados adicionales comentados por ahora
	//m_rasterizer.destroy();
	//m_blendstate.destroy();

	// Liberar sampler
	m_sampler.destroy();
}

/// <summary>
/// Asigna las mallas del actor y crea los vertex/index buffers
/// correspondientes en GPU.
/// </summary>
/// <param name="device">Dispositivo usado para crear los buffers.</param>
/// <param name="meshes">Colección de mallas que formarán el modelo del actor.</param>
void
Actor::setMesh(Device& device, std::vector<MeshComponent> meshes) {
	m_meshes = meshes;
	HRESULT hr;

	for (auto& mesh : m_meshes) {
		// Crear vertex buffer
		Buffer vertexBuffer;
		hr = vertexBuffer.init(device, mesh, D3D11_BIND_VERTEX_BUFFER);
		if (FAILED(hr)) {
			ERROR("Actor", "setMesh", "Failed to create new vertexBuffer");
		}
		else {
			m_vertexBuffers.push_back(vertexBuffer);
		}

		// Crear index buffer
		Buffer indexBuffer;
		hr = indexBuffer.init(device, mesh, D3D11_BIND_INDEX_BUFFER);
		if (FAILED(hr)) {
			ERROR("Actor", "setMesh", "Failed to create new indexBuffer");
		}
		else {
			m_indexBuffers.push_back(indexBuffer);
		}
	}
}
