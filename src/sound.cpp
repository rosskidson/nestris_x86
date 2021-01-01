#include "sound.hpp"

#include <memory>
#include <stdexcept>

#include "Extensions/olcPGEX_Sound.h"
#include "utils/logging.hpp"

namespace sound {
SoundPlayer::SoundPlayer() {
  olc::SOUND::InitialiseAudio(44100, 1, 8, 512);
}

SoundPlayer::~SoundPlayer() {}

bool SoundPlayer::loadWavFromFilesystem(const std::string& path, const std::string& sample_name) {
  const auto sample_id = olc::SOUND::LoadAudioSample(path);
  if (sample_id == -1) {
    LOG_ERROR("Failed loading sample `" << path << "`.");
    return false;
  }
  samples_[sample_name] = sample_id;
  return true;
}

void SoundPlayer::loadWavFromMemory(const olc::AudioSample& sample,
                                    const std::string& sample_name) {
  samples_[sample_name] = olc::SOUND::LoadAudioSample(sample);
}

bool SoundPlayer::playSample(const std::string& sample_name) const {
  if (not samples_.count(sample_name)) {
    LOG_ERROR("No sample found for identifier `" << sample_name << "`.");
    return false;
  }
  olc::SOUND::PlaySample(samples_.at(sample_name));
  return true;
}
}  // namespace sound
