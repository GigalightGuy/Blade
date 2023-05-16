#include "AudioSource.hpp"
#include "AudioManager.hpp"

#define MAX_VOLUME 2
#define MIN_VOLUME 0

#define MAX_PITCH 1
#define MIN_PITCH -1

AudioSource::AudioSource(AudioClip *clip) {
  this->clip = clip;
  volume = (MAX_VOLUME - MIN_VOLUME) / 2.0f;
  pitch = MIN_PITCH + (MAX_PITCH - MIN_PITCH) / 2.0f;
  looping = false;
  UpdateValues();
}

AudioSource::~AudioSource() {}

void AudioSource::Play() { ma_sound_start(&clip->sound); }

void AudioSource::Stop() { ma_sound_stop(&clip->sound); }

void AudioSource::SetVolume(float volume) {
  if (volume < MIN_VOLUME || volume > MAX_VOLUME) {
    return;
  }
  this->volume = volume;
  UpdateValues();
}

void AudioSource::SetPitch(float pitch) {
  if (pitch < MIN_PITCH || pitch > MAX_PITCH) {
    return;
  }
  this->pitch = pitch;
  UpdateValues();
}

void AudioSource::SetLooping(float state) {
  this->looping = state;
  UpdateValues();
}

void AudioSource::UpdateValues() {
  ma_sound_set_volume(&clip->sound, volume);
  ma_sound_set_pitch(&clip->sound, pitch);
  ma_sound_set_looping(&clip->sound, looping);
}
