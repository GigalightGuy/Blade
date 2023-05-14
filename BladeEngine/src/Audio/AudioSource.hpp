#pragma once
#include "AudioClip.hpp"

class AudioSource {
public:
  AudioSource(AudioClip *clip);
  ~AudioSource();

  void Play();
  void Stop();

private:
  AudioClip *clip;
};
