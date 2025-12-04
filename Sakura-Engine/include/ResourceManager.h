#pragma once
#include "Prerequisites.h"
#include "IResource.h"

/// <summary>
/// Administrador global de recursos (modelos, texturas, shaders, etc.).
/// Implementa un caché tipo singleton para reutilizar instancias cargadas.
/// </summary>
class
	ResourceManager {
public:
	/// <summary>
	/// Constructor por defecto.
	/// </summary>
	ResourceManager() = default;

	/// <summary>
	/// Destructor por defecto.
	/// </summary>
	~ResourceManager() = default;

	/// <summary>
	/// Acceso al singleton de ResourceManager.
	/// </summary>
	/// <returns>Referencia única al administrador de recursos.</returns>
	static ResourceManager& getInstance() {
		static ResourceManager instance;
		return instance;
	}

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	/// <summary>
	/// Obtiene un recurso de tipo T o lo carga si aún no existe en caché.
	/// T debe heredar de IResource.
	/// </summary>
	/// <typeparam name="T">Tipo de recurso (derivado de IResource).</typeparam>
	/// <typeparam name="Args">Parámetros adicionales para el constructor de T.</typeparam>
	/// <param name="key">Clave con la que se identifica el recurso en el caché.</param>
	/// <param name="filename">Archivo desde el cual se carga el recurso.</param>
	/// <param name="args">Argumentos adicionales para construir el recurso.</param>
	/// <returns>Puntero compartido al recurso cargado o existente; nullptr si falla.</returns>
	template<typename T, typename... Args>
	std::shared_ptr<T> GetOrLoad(const std::string& key,
		const std::string& filename,
		Args&&... args) {
		static_assert(std::is_base_of<IResource, T>::value,
			"T debe heredar de IResource");
		// 1. ¿Ya existe el recurso en el caché?
		auto it = m_resources.find(key);
		if (it != m_resources.end()) {
			// Intentar castear al tipo correcto
			auto existing = std::dynamic_pointer_cast<T>(it->second);
			if (existing && existing->GetState() == ResourceState::Loaded) {
				return existing; // Flyweight: reutiliza la instancia ya cargada
			}
		}

		// 2. No existe o no está cargado -> crearlo y cargarlo
		std::shared_ptr<T> resource = std::make_shared<T>(key, std::forward<Args>(args)...);

		if (!resource->load(filename)) {
			// Manejo básico de error de carga
			return nullptr;
		}

		if (!resource->init()) {
			// Manejo básico de error de inicialización
			return nullptr;
		}

		// 3. Guardar en el caché y devolver
		m_resources[key] = resource;
		return resource;
	}

	/// <summary>
	/// Obtiene un recurso ya cargado sin intentar cargarlo si no existe.
	/// </summary>
	/// <typeparam name="T">Tipo de recurso esperado.</typeparam>
	/// <param name="key">Clave del recurso en el caché.</param>
	/// <returns>Puntero compartido al recurso o nullptr si no se encuentra.</returns>
	template<typename T>
	std::shared_ptr<T> Get(const std::string& key) const
	{
		auto it = m_resources.find(key);
		if (it == m_resources.end()) return nullptr;

		return std::dynamic_pointer_cast<T>(it->second);
	}

	/// <summary>
	/// Libera un recurso específico del caché y llama a unload().
	/// </summary>
	/// <param name="key">Clave del recurso a liberar.</param>
	void Unload(const std::string& key)
	{
		auto it = m_resources.find(key);
		if (it != m_resources.end()) {
			it->second->unload();
			m_resources.erase(it);
		}
	}

	/// <summary>
	/// Libera todos los recursos almacenados en el caché.
	/// </summary>
	void UnloadAll()
	{
		for (auto& [key, res] : m_resources) {
			if (res) {
				res->unload();
			}
		}
		m_resources.clear();
	}

private:
	// Caché de recursos, indexados por una clave de texto.
	std::unordered_map<std::string, std::shared_ptr<IResource>> m_resources;
};
