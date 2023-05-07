#include "PlayerSystems.hpp"

void Move(flecs::entity e, const Controller &ctrl, BladeEngine::Rigidbody2D &rb,
          const BladeEngine::Position &pos) {
  float desiredVel = 0.0f;
  desiredVel += BladeEngine::Input::GetKey(ctrl.Left) ? -ctrl.Movespeed : 0.0f;
  desiredVel += BladeEngine::Input::GetKey(ctrl.Right) ? ctrl.Movespeed : 0.0f;

  BladeEngine::Vec2 currentVel = BladeEngine::Physics2D::GetBodyVelocity(rb);
  float velChange = desiredVel - currentVel.X;
  float impulse = BladeEngine::Physics2D::GetBodyMass(rb) * velChange;

  // BladeEngine::Physics2D::AddImpulse(rb, BladeEngine::Vec2(impulse, 0.0f));
  // BladeEngine::Physics2D::AddForce(rb,BladeEngine::Vec2(impulse, 0.0f));
  BladeEngine::Physics2D::SetVelocity2D(rb,BladeEngine::Vec2(impulse, 0.0f),1);
}