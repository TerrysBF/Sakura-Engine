#pragma once
#include "Prerequisites.h"
#include "MeshComponent.h"

// Forward declarations para no incluir los headers completos aquí
class Device;
class DeviceContext;

/**
 * Clase Buffer
 *
 * Esta clase envuelve un ID3D11Buffer de Direct3D.
 * Sirve para manejar vertex buffers, index buffers y constant buffers.
 */
class Buffer {
public:
  // Constructor por defecto, no crea nada todavía
  Buffer() = default;

  // Destructor por defecto, no libera nada si no se llama a destroy()
  ~Buffer() = default;

  /**
   * Inicializa el buffer usando un MeshComponent.
   *
   * Según el bindFlag el buffer se usa como:
   * - D3D11_BIND_VERTEX_BUFFER  -> guarda vértices
   * - D3D11_BIND_INDEX_BUFFER   -> guarda índices
   */
  HRESULT
    init(Device& device, const MeshComponent& mesh, unsigned int bindFlag);

  /**
   * Inicializa el buffer como constant buffer.
   *
   * Se crea un buffer de tamaño ByteWidth con flag D3D11_BIND_CONSTANT_BUFFER.
   * Normalmente se usa para mandar matrices u otras constantes al shader.
   */
  HRESULT
    init(Device& device, unsigned int ByteWidth);

  /**
   * Actualiza el contenido del buffer.
   *
   * Normalmente se usa para escribir datos nuevos cada frame (por ejemplo matrices).
   * Internamente llama a UpdateSubresource del device context.
   */
  void
    update(DeviceContext& deviceContext,
      ID3D11Resource* pDstResource,
      unsigned int    DstSubresource,
      const D3D11_BOX* pDstBox,
      const void* pSrcData,
      unsigned int    SrcRowPitch,
      unsigned int    SrcDepthPitch);

  /**
   * Enlaza el buffer al pipeline de render.
   *
   * El comportamiento depende del tipo de buffer:
   * - Si es vertex buffer: se llama a IASetVertexBuffers.
   * - Si es index buffer: se llama a IASetIndexBuffer.
   * - Si es constant buffer: se enlaza al VS y opcionalmente al PS.
   */
  void
    render(DeviceContext& deviceContext,
      unsigned int   StartSlot,
      unsigned int   NumBuffers,
      bool           setPixelShader = false,
      DXGI_FORMAT    format = DXGI_FORMAT_UNKNOWN);

  /**
   * Libera el ID3D11Buffer interno.
   *
   * Deja el puntero en nullptr y resetea los valores básicos.
   */
  void
    destroy();

  /**
   * Función de ayuda para crear un buffer genérico.
   *
   * Recibe la descripción del buffer y datos iniciales opcionales.
   * Normalmente solo la usan las funciones init internas.
   */
  HRESULT
    createBuffer(Device& device,
      D3D11_BUFFER_DESC& desc,
      D3D11_SUBRESOURCE_DATA* initData);

private:
  // Puntero al buffer de Direct3D (vértices, índices o constantes)
  ID3D11Buffer* m_buffer = nullptr;

  // Tamaño de cada elemento en bytes (solo importa en vertex buffers)
  unsigned int m_stride = 0;

  // Desplazamiento inicial en bytes para el buffer (normalmente 0)
  unsigned int m_offset = 0;

  // Bandera que indica cómo se usa el buffer (D3D11_BIND_VERTEX_BUFFER, etc.)
  unsigned int m_bindFlag = 0;
};
