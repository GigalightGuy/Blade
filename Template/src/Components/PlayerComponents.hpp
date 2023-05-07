#pragma once

#include "Core/Game.hpp"

struct Player {};

struct Controller {
  BladeEngine::KeyCode Left;
  BladeEngine::KeyCode Right;

  float Movespeed;
};