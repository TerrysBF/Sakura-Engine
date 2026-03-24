#pragma once
#include "Prerequisites.h"
#include "IResource.h"
#include "MeshComponent.h"
#include "fbxsdk.h"

/**
 * @brief Tipos de modelos que nuestro motor sabe leer.
 */
enum ModelType {
	OBJ,  ///< Formato simple (solo geometría).
	FBX   ///< Formato avanzado (puede incluir animaciones, luces y materiales).
};

/**
 * @class Model3D
 * @brief Representa un objeto 3D completo en el juego.
 * * Un "Model3D" es como una caja que contiene una o varias "Mallas" (Meshes).
 * Por ejemplo, un modelo de un coche puede tener una malla para el chasis y cuatro para las ruedas.
 * Esta clase se encarga de cargar esos archivos desde el disco duro a la memoria.
 */
class Model3D : public IResource {
public:
	/**
	 * @brief Crea un modelo dándole un nombre y diciendo qué tipo de archivo es.
	 * @param name Ruta del archivo (ej: "Assets/Models/Espada.fbx").
	 * @param modelType Si es un archivo OBJ o FBX.
	 */
	Model3D(const std::string& name, ModelType modelType)
		: IResource(name), m_modelType(modelType), lSdkManager(nullptr), lScene(nullptr) {
		SetType(ResourceType::Model3D);
		load(name); // Intenta cargarlo en cuanto se crea
	}

	~Model3D() = default;

	/**
	 * @brief Abre el archivo y extrae la información 3D.
	 * @param path Ruta donde está guardado el modelo.
	 * @return true si el archivo se leyó correctamente.
	 */
	bool load(const std::string& path) override;

	/**
	 * @brief Prepara el modelo para ser usado en el motor.
	 */
	bool init() override;

	/**
	 * @brief Borra los datos del modelo cuando ya no lo necesitamos para liberar RAM.
	 */
	void unload() override;

	/**
	 * @brief Calcula cuánto espacio ocupa este modelo en la memoria (en bytes).
	 */
	size_t getSizeInBytes() const override;

	/**
	 * @brief Nos da la lista de todas las mallas (partes) que forman este modelo.
	 */
	const std::vector<MeshComponent>& GetMeshes() const { return m_meshes; }

	/* --- FUNCIONES PARA CARGAR FBX --- */

	/**
	 * @brief Enciende el "motor de lectura" de FBX (el SDK de Autodesk).
	 */
	bool InitializeFBXManager();

	/**
	 * @brief Función maestra que lee un archivo FBX y lo convierte en mallas de nuestro motor.
	 */
	std::vector<MeshComponent> LoadFBXModel(const std::string& filePath);

	/**
	 * @brief Recorre el "árbol" del archivo FBX buscando piezas (nodos).
	 * * Los archivos FBX son como carpetas dentro de carpetas; esta función busca en todas.
	 */
	void ProcessFBXNode(FbxNode* node);

	/**
	 * @brief Extrae los puntos (vértices) y caras de una malla FBX.
	 */
	void ProcessFBXMesh(FbxNode* node);

	/**
	 * @brief Busca qué texturas o colores usa el modelo FBX.
	 */
	void ProcessFBXMaterials(FbxSurfaceMaterial* material);

	/**
	 * @brief Devuelve una lista con los nombres de las imágenes (texturas) que necesita este modelo.
	 */
	std::vector<std::string> GetTextureFileNames() const { return textureFileNames; }

private:
	// Herramientas internas del lector de FBX (SDK de Autodesk)
	FbxManager* lSdkManager; ///< El jefe que administra la lectura.
	FbxScene* lScene;       ///< El contenedor de toda la escena del archivo.

	std::vector<std::string> textureFileNames; ///< Lista de rutas de texturas encontradas.

public:
	ModelType m_modelType;                ///< ¿Es OBJ o FBX?
	std::vector<MeshComponent> m_meshes;  ///< Las partes reales del objeto que se dibujarán.
};