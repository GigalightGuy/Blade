#pragma once
#include "AudioClip.hpp"
#include "miniaudio.h"
#include <vector>

struct AudioManager {
  static ma_engine engine;
  static std::vector<AudioClip *> audioClips;

  static void Init();
  static void Shutdown();
};
