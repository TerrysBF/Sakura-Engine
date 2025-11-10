#pragma once
#include "Prerequisites.h"

class Device;
class DeviceContext;

// Clase que envuelve un ID3D11SamplerState.
// Un sampler dice cómo se leen las texturas en los shaders:
// - tipo de filtrado (linear, point, etc.)
// - cómo se repite la textura (wrap, clamp, etc.)
class SamplerState {
public:
  // Constructor por defecto, no hace nada especial.
  SamplerState() = default;

  // Destructor por defecto.
  // No libera el recurso por sí solo, para eso está destroy().
  ~SamplerState() = default;

  // Crea el sampler con una configuración básica.
  // Aquí se arma el D3D11_SAMPLER_DESC (filtrado linear, wrap en U/V/W, etc.)
  // y se llama a CreateSamplerState.
  // Devuelve S_OK si todo salió bien o un HRESULT de error.
  HRESULT init(Device& device);

  // Función de update (por ahora no hace nada).
  // Se podría usar si luego queremos cambiar la configuración del sampler.
  void update();

  // Enlaza el sampler al Pixel Shader.
  // Llama a PSSetSamplers en el device context para que los shaders
  // puedan usar este sampler cuando leen texturas.
  // StartSlot es el slot inicial y NumSamplers normalmente es 1.
  void render(DeviceContext& deviceContext,
    unsigned int StartSlot,
    unsigned int NumSamplers);

  // Libera el sampler.
  // Usa SAFE_RELEASE, así que se puede llamar varias veces sin problema.
  void destroy();

public:
  // Puntero al ID3D11SamplerState real.
  // Se crea en init() y se hace Release en destroy().
  ID3D11SamplerState* m_sampler = nullptr;
};
