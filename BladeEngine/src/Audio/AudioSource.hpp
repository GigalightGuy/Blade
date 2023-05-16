#pragma once
#include "AudioClip.hpp"

class AudioSource {
public:
  AudioSource(AudioClip *clip);
  ~AudioSource();

  void Play();
  void Stop();

  void SetVolume(float volume);
  void SetPitch(float pitch);
  void SetLooping(float state);

private:
  void UpdateValues();

  float volume; // dB
  float pitch;  // Hz

  bool looping;

  AudioClip *clip;
};
