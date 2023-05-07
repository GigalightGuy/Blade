#pragma once

#include "Core/Game.hpp"

struct Player {
  int points;
};

struct WaterBar{};

struct Controller {
  BladeEngine::KeyCode Left;
  BladeEngine::KeyCode Right;

  float Movespeed;
};