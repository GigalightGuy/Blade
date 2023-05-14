#include "AudioManager.hpp"
#include <iostream>

ma_engine AudioManager::engine;
std::vector<AudioClip *> AudioManager::audioClips;

void AudioManager::Init() {
  ma_result result = ma_engine_init(NULL, &engine);
  if (result != MA_SUCCESS) {
    std::cout << "Failed to initialize audio engine." << std::endl;
  }
}

void AudioManager::Shutdown() {
  for (size_t i = 0; i < audioClips.size(); i++) {
    DisposeAudioClip(*audioClips.at(i));
  }
  ma_engine_uninit(&engine);
}
