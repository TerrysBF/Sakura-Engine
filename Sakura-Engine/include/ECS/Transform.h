#pragma once
#include "Prerequisites.h"
#include "EngineUtilities/Vectors/Vector3.h"
#include "ECS/Component.h"

class DeviceContext;

/**
 * @class Transform
 * @brief Componente de transformación (posición, rotación, escala + matriz).
 */
class Transform : public Component
{
public:
  /// Constructor: inicializa en origen y escala 1, y marca tipo TRANSFORM.
  Transform()
    : position()
    , rotation()
    , scale()
    , matrix()
    , Component(ComponentType::TRANSFORM)
  {
  }

  /// Inicializa el componente (escala 1 y matriz identidad).
  void init() override {
    // Escala (1,1,1)
    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = 1.0f;

    matrix = XMMatrixIdentity();
  }

  /// Actualiza la matriz a partir de position/rotation/scale.
  void update(float /*deltaTime*/) override
  {
    XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
    XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);

    // Orden típico: S * R * T
    matrix = S * R * T;
  }

  /// No hace nada por ahora; el transform se usa desde otros sistemas.
  void render(DeviceContext& /*deviceContext*/) override {}

  /// No hay recursos GPU, así que no hay nada que liberar.
  void destroy() override {}

  // --------- Get / Set ---------

  const EU::Vector3& getPosition() const { return position; }
  void setPosition(const EU::Vector3& newPos) { position = newPos; }

  const EU::Vector3& getRotation() const { return rotation; }
  void setRotation(const EU::Vector3& newRot) { rotation = newRot; }

  const EU::Vector3& getScale() const { return scale; }
  void setScale(const EU::Vector3& newScale) { scale = newScale; }

  /// Set de todo el transform de golpe.
  void setTransform(const EU::Vector3& newPos,
    const EU::Vector3& newRot,
    const EU::Vector3& newSca)
  {
    position = newPos;
    rotation = newRot;
    scale = newSca;
  }

  /// Traslada la posición en los tres ejes.
  void translate(const EU::Vector3& translation)
  {
    position.x += translation.x;
    position.y += translation.y;
    position.z += translation.z;
  }

public:
  XMMATRIX matrix;   ///< Matriz compuesta S*R*T (pública para usarla rápido).

private:
  EU::Vector3 position;  ///< Posición (x,y,z)
  EU::Vector3 rotation;  ///< Rotación (pitch,yaw,roll) en radianes
  EU::Vector3 scale;     ///< Escala (x,y,z)
};
