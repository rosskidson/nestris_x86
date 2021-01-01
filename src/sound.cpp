#include "sound.hpp"

#include <memory>
#include <stdexcept>

#include "olcPixelGameEngine.h"
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

// bool SoundPlayer::loadWavFromMemory(std::unique_ptr<Mix_Chunk>&& sample,
//                                    const std::string& sample_name) {
//  if(not sample) {
//    LOG_ERROR("Sample named `" << sample_name << "` came in to loadWavFromMemory as a nullptr.");
//    return false;
//  }

//  samples_[sample_name] = std::move(sample);
//  return true;
//}

bool SoundPlayer::playSample(const std::string& sample_name) const {
  if (not samples_.count(sample_name)) {
    LOG_ERROR("No sample found for identifier `" << sample_name << "`.");
    return false;
  }
  olc::SOUND::PlaySample(samples_.at(sample_name));
  return true;
}
}  // namespace sound
