#include "AudioSource.hpp"
#include "AudioManager.hpp"

AudioSource::AudioSource(AudioClip *clip) { this->clip = clip; }

AudioSource::~AudioSource() {}

void AudioSource::Play() { ma_sound_start(&clip->sound); }

void AudioSource::Stop() { ma_sound_stop(&clip->sound); }
