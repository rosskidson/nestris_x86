#pragma once

#include <map>
#include <memory>
#include <string>

namespace olc {
class AudioSample;
}

namespace sound {
class SoundPlayer {
 public:
  SoundPlayer();

  ~SoundPlayer();

  [[nodiscard]] bool loadWavFromFilesystem(const std::string& path, const std::string& sample_name);

  void loadWavFromMemory(const olc::AudioSample& sample, const std::string& sample_name);

  bool playSample(const std::string& sample_name) const;

 private:
  std::map<std::string, int> samples_;
};
}  // namespace sound
