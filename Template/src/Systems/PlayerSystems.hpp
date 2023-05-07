#pragma once

#include "BladeEngine.hpp"
#include "../Components/PlayerComponents.hpp"

void Move(flecs::entity e, const Controller &ctrl, BladeEngine::Rigidbody2D &rb,
          const BladeEngine::Position &pos);