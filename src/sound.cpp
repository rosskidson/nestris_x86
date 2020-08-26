#include "sound.hpp"

#include <SDL_mixer.h>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace sound {
SoundPlayer::SoundPlayer() {
  const int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
  if(result < 0) {
    throw std::runtime_error("Failed to initialize SDL sound mixer.");
  }
}

SoundPlayer::~SoundPlayer() { Mix_CloseAudio(); }

bool SoundPlayer::loadWavFromFilesystem(const std::string& path,
                                        const std::string& sample_name) {
  const auto sample = Mix_LoadWAV(path.c_str());
  if(sample == nullptr) {
    std::cerr << "Failed loading sample `" << path << "`." << std::endl;
    return false;
  }

  samples_[sample_name] = std::unique_ptr<Mix_Chunk>(sample);
  return true;
}

bool SoundPlayer::playSample(const std::string& sample_name) const {
  if(not samples_.count(sample_name)) {
    std::cout << "No sample found for identifier `" << sample_name << "`." << std::endl;
    return false;
  }
  Mix_PlayChannel(-1, samples_.at(sample_name).get(), 0);
  return true;
}
}  // namespace sound
