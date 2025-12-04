#pragma once
#include "Prerequisites.h"
#include "IResource.h"
#include "MeshComponent.h"
#include "fbxsdk.h"

/// <summary>
/// Tipos de modelo soportados por el cargador 3D.
/// </summary>
enum
	ModelType {
	OBJ,  ///< Modelo en formato .obj.
	FBX   ///< Modelo en formato .fbx (usando FBX SDK).
};

/// <summary>
/// Recurso que representa un modelo 3D compuesto por una o varias mallas.
/// Puede cargar modelos en formato OBJ o FBX y almacenarlos como MeshComponent.
/// </summary>
class
	Model3D : public IResource {
public:
	/// <summary>
	/// Construye un modelo 3D a partir de un nombre de archivo y un tipo de modelo.
	/// </summary>
	/// <param name="name">Ruta o nombre del archivo del modelo.</param>
	/// <param name="modelType">Tipo de modelo (OBJ o FBX).</param>
	Model3D(const std::string& name, ModelType modelType)
		: IResource(name), m_modelType(modelType), lSdkManager(nullptr), lScene(nullptr) {
		SetType(ResourceType::Model3D);
		load(name);
	}

	/// <summary>
	/// Destructor por defecto.
	/// </summary>
	~Model3D() = default;

	/// <summary>
	/// Carga el modelo desde disco según el tipo especificado (OBJ/FBX).
	/// </summary>
	/// <param name="path">Ruta del archivo de modelo.</param>
	/// <returns>true si la carga fue exitosa; false en caso contrario.</returns>
	bool
		load(const std::string& path) override;

	/// <summary>
	/// Inicializa el recurso de modelo 3D.
	/// </summary>
	/// <returns>true si la inicialización fue exitosa; false en caso contrario.</returns>
	bool
		init() override;

	/// <summary>
	/// Libera los datos asociados al modelo (mallas, texturas, etc.).
	/// </summary>
	void
		unload() override;

	/// <summary>
	/// Devuelve el tamaño estimado del modelo en memoria, en bytes.
	/// </summary>
	/// <returns>Tamaño en bytes.</returns>
	size_t
		getSizeInBytes() const override;

	/// <summary>
	/// Obtiene el arreglo de mallas cargadas para este modelo.
	/// </summary>
	/// <returns>Referencia constante al vector de MeshComponent.</returns>
	const std::vector<MeshComponent>&
		GetMeshes() const { return m_meshes; }

	/* FBX MODEL LOADER*/

	/// <summary>
	/// Inicializa el administrador de FBX (lSdkManager) y la escena (lScene).
	/// </summary>
	/// <returns>true si la inicialización del FBX SDK fue correcta.</returns>
	bool
		InitializeFBXManager();

	/// <summary>
	/// Carga un modelo FBX desde archivo y devuelve sus mallas procesadas.
	/// </summary>
	/// <param name="filePath">Ruta del archivo FBX.</param>
	/// <returns>Vector de MeshComponent construidos a partir del modelo FBX.</returns>
	std::vector<MeshComponent>
		LoadFBXModel(const std::string& filePath);

	/// <summary>
	/// Procesa recursivamente un nodo FBX y sus hijos para extraer mallas.
	/// </summary>
	/// <param name="node">Nodo raíz o actual dentro de la escena FBX.</param>
	void
		ProcessFBXNode(FbxNode* node);

	/// <summary>
	/// Procesa un nodo que contiene una malla FBX y la convierte en MeshComponent.
	/// </summary>
	/// <param name="node">Nodo que contiene la malla FBX.</param>
	void
		ProcessFBXMesh(FbxNode* node);

	/// <summary>
	/// Procesa el material de una malla FBX para extraer información relevante
	/// (por ejemplo, nombres de texturas).
	/// </summary>
	/// <param name="material">Puntero al material de la malla FBX.</param>
	void
		ProcessFBXMaterials(FbxSurfaceMaterial* material);

	/// <summary>
	/// Devuelve la lista de nombres de archivos de textura encontrados en el modelo FBX.
	/// </summary>
	/// <returns>Vector con nombres de archivo de texturas.</returns>
	std::vector<std::string>
		GetTextureFileNames() const { return textureFileNames; }

private:
	// Punteros principales del FBX SDK (administrador y escena).
	FbxManager* lSdkManager;
	FbxScene* lScene;

	// Nombres de archivos de textura encontrados al procesar el FBX.
	std::vector<std::string> textureFileNames;

public:
	ModelType m_modelType;                  // Tipo de modelo (OBJ o FBX).
	std::vector<MeshComponent> m_meshes;    // Mallas resultantes después de cargar el modelo.
};
