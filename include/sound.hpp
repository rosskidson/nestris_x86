#pragma once

#include <map>
#include <memory>
#include <string>

namespace sound {
class SoundPlayer {
 public:
  SoundPlayer();

  ~SoundPlayer();

  [[nodiscard]] bool loadWavFromFilesystem(const std::string& path, const std::string& sample_name);

  //[[nodiscard]] bool loadWavFromMemory(std::unique_ptr<Mix_Chunk>&& sample,
  //                                     const std::string& sample_name);

  bool playSample(const std::string& sample_name) const;

 private:
  std::map<std::string, int> samples_;
};
}  // namespace sound
