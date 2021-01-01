#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "olcPixelGameEngine.h"
#include "sound.hpp"

constexpr bool LOAD_FROM_BINARY = true;

namespace nestris_x86 {

class SpriteProvider {
 public:
  SpriteProvider();
  SpriteProvider(const std::string &path);

  bool loadSprites(const std::string &path);
  bool loadSprites();

  olc::Sprite *getSprite(const std::string &sprite_name) const;

 private:
  std::map<std::string, std::unique_ptr<olc::Sprite>> sprite_map_;
};

bool loadSoundAssets(const std::string &path, sound::SoundPlayer &sample_player);
bool loadSoundAssets(sound::SoundPlayer &sample_player);

}  // namespace nestris_x86
