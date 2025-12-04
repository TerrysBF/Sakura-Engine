#pragma once
#include "Prerequisites.h"

/// <summary>
/// Tipo de recurso manejado por el sistema (modelo, textura, shader, etc.).
/// </summary>
enum class
	ResourceType {
	Unknown,  // Tipo desconocido o no inicializado.
	Model3D,  // Recurso de modelo 3D.
	Texture,  // Recurso de textura.
	Sound,    // Recurso de sonido.
	Shader,   // Recurso de shader.
	Material  // Recurso de material.
};

/// <summary>
/// Estado actual de un recurso en memoria o durante su ciclo de vida.
/// </summary>
enum class
	ResourceState {
	Unloaded, // No cargado.
	Loading,  // En proceso de carga.
	Loaded,   // Cargado y listo para usarse.
	Failed    // Falló la carga o inicialización.
};

/// <summary>
/// Interfaz base para todos los recursos del motor (modelos, texturas, shaders, etc.).
/// Define el ciclo de vida básico: inicializar, cargar, descargar y reportar tamaño.
/// </summary>
class IResource {
public:
	/// <summary>
	/// Construye un recurso con un nombre lógico.
	/// Inicialmente se marca como tipo desconocido y estado Unloaded.
	/// </summary>
	/// <param name="name">Nombre lógico del recurso.</param>
	IResource(const std::string& name)
		: m_name(name)
		, m_filePath("")
		, m_type(ResourceType::Unknown)
		, m_state(ResourceState::Unloaded)
		, m_id(GenerateID())
	{	}

	/// <summary>
	/// Destructor virtual por defecto.
	/// </summary>
	virtual ~IResource() = default;

	/// <summary>
	/// Crea o inicializa el recurso en GPU (buffers, texturas, etc.).
	/// </summary>
	/// <returns>true si la inicialización fue exitosa; false en caso contrario.</returns>
	virtual bool init() = 0;

	/// <summary>
	/// Carga el recurso desde disco usando una ruta de archivo.
	/// </summary>
	/// <param name="filename">Ruta o nombre de archivo del recurso.</param>
	/// <returns>true si la carga fue exitosa; false en caso contrario.</returns>
	virtual bool load(const std::string& filename) = 0;

	/// <summary>
	/// Libera la memoria asociada al recurso (CPU/GPU).
	/// </summary>
	virtual void unload() = 0;

	/// <summary>
	/// Devuelve el tamaño estimado del recurso en memoria (en bytes).
	/// Útil para profiling o administración de memoria.
	/// </summary>
	/// <returns>Tamaño aproximado en bytes.</returns>
	virtual size_t getSizeInBytes() const = 0;

	// Ruta de archivo asociada al recurso.
	void SetPath(const std::string& path) { m_filePath = path; }

	// Tipo de recurso (modelo, textura, etc.).
	void SetType(ResourceType t) { m_type = t; }

	// Estado actual del recurso (Loaded, Failed, etc.).
	void SetState(ResourceState s) { m_state = s; }

	// Getters básicos del recurso.
	const std::string& GetName() const { return m_name; }
	const std::string& GetPath() const { return m_filePath; }
	ResourceType GetType() const { return m_type; }
	ResourceState GetState() const { return m_state; }
	uint64_t GetID() const { return m_id; }

protected:
	// Nombre lógico del recurso.
	std::string m_name;

	// Ruta del archivo desde el que se carga el recurso.
	std::string m_filePath;

	// Tipo de recurso.
	ResourceType m_type;

	// Estado del recurso en su ciclo de vida.
	ResourceState m_state;

	// Identificador único del recurso.
	uint64_t m_id;

private:
	/// <summary>
	/// Genera un identificador incremental único para cada recurso.
	/// </summary>
	/// <returns>ID único.</returns>
	static uint64_t GenerateID()
	{
		static uint64_t nextID = 1;
		return nextID++;
	}
};
