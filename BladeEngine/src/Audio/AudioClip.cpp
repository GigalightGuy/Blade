#include "AudioClip.hpp"
#include "AudioManager.hpp"
#include <iostream>

AudioClip* LoadAudioClip(const char *path) {
  AudioClip *clip = new AudioClip();

  ma_result result = ma_sound_init_from_file(&AudioManager::engine, path, 0,
                                             NULL, NULL, &clip->sound);
  if (result != MA_SUCCESS) {
    std::cout << "Failed to Load audio clip." << std::endl;
  }

  AudioManager::audioClips.push_back(clip);

  return clip;
}

void DisposeAudioClip(AudioClip audioClip) {
  ma_sound_uninit(&audioClip.sound);
}
