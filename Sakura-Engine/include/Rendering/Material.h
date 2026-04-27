/**
 * @file Material.h
 * @brief Define la clase Material usada en el sistema de rendering.
 */
#pragma once
#include "Prerequisites.h"
#include "Rendering/RenderTypes.h"

class ShaderProgram;
class RasterizerState;
class DepthStencilState;
class SamplerState;

/**
 * @class Material
 * @brief Representa la configuración visual de un objeto al renderizarse.
 *
 * Un Material agrupa el shader, estados de render y tipo de mezcla
 * que se usan al dibujar una superficie.
 */
class
	Material {
public:
	// Asigna el shader del material
	void setShader(ShaderProgram* shader) { m_shader = shader; }

	// Asigna el estado de rasterización
	void setRasterizerState(RasterizerState* state) { m_rasterizerState = state; }

	// Asigna el estado de profundidad
	void setDepthStencilState(DepthStencilState* state) { m_depthStencilState = state; }

	// Asigna el sampler para texturas
	void setSamplerState(SamplerState* state) { m_samplerState = state; }

	// Define el tipo de material (opaco, transparente, etc.)
	void setDomain(MaterialDomain domain) { m_domain = domain; }

	// Define el modo de mezcla
	void setBlendMode(BlendMode blendMode) { m_blendMode = blendMode; }

	// Obtiene el shader del material
	ShaderProgram* getShader() const { return m_shader; }

	// Obtiene el estado de rasterización
	RasterizerState* getRasterizerState() const { return m_rasterizerState; }

	// Obtiene el estado de profundidad
	DepthStencilState* getDepthStencilState() const { return m_depthStencilState; }

	// Obtiene el sampler
	SamplerState* getSamplerState() const { return m_samplerState; }

	// Obtiene el dominio del material
	MaterialDomain getDomain() const { return m_domain; }

	// Obtiene el modo de mezcla
	BlendMode getBlendMode() const { return m_blendMode; }

private:
	ShaderProgram* m_shader = nullptr;                // Shader principal
	RasterizerState* m_rasterizerState = nullptr;     // Estado de rasterización
	DepthStencilState* m_depthStencilState = nullptr; // Estado de profundidad
	SamplerState* m_samplerState = nullptr;           // Sampler de texturas
	MaterialDomain m_domain = MaterialDomain::Opaque; // Tipo de material
	BlendMode m_blendMode = BlendMode::Opaque;        // Modo de mezcla
};