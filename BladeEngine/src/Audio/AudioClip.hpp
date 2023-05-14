#pragma once
#include "miniaudio.h"

struct AudioClip {
  ma_sound sound;
};

AudioClip *LoadAudioClip(const char *path);
void DisposeAudioClip(AudioClip audioClip);
