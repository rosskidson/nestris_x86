#include "sound.hpp"

#include <SDL_mixer.h>

#include <memory>
#include <stdexcept>

#include "utils/logging.hpp"

namespace sound {
SoundPlayer::SoundPlayer() {
  const int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
  if (result < 0) {
    throw std::runtime_error("Failed to initialize SDL sound mixer.");
  }
}

SoundPlayer::~SoundPlayer() {
  Mix_CloseAudio();
}

bool SoundPlayer::loadWavFromFilesystem(const std::string& path, const std::string& sample_name) {
  const auto sample = Mix_LoadWAV(path.c_str());
  if (sample == nullptr) {
    LOG_ERROR("Failed loading sample `" << path << "`.");
    return false;
  }

  samples_[sample_name] = std::unique_ptr<Mix_Chunk>(sample);
  return true;
}

bool SoundPlayer::loadWavFromMemory(std::unique_ptr<Mix_Chunk>&& sample,
                                    const std::string& sample_name) {
  if(not sample) {
    LOG_ERROR("Sample named `" << sample_name << "` came in to loadWavFromMemory as a nullptr.");
    return false;
  }

  samples_[sample_name] = std::move(sample);
  return true;
}

bool SoundPlayer::playSample(const std::string& sample_name) const {
  if (not samples_.count(sample_name)) {
    LOG_ERROR("No sample found for identifier `" << sample_name << "`.");
    return false;
  }
  Mix_PlayChannel(-1, samples_.at(sample_name).get(), 0);
  return true;
}
}  // namespace sound
