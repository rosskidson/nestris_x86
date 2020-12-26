#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "sound.hpp"
#include "olcPixelGameEngine.h"

namespace tetris_clone {

class SpriteProvider {
 public:
  SpriteProvider();
  SpriteProvider(const std::string& path);

  bool loadSprites(const std::string &path);
  bool loadSprites();

  olc::Sprite *getSprite(const std::string &sprite_name) const;

 private:
  std::map<std::string, std::unique_ptr<olc::Sprite>> sprite_map_;
};

bool loadSoundAssets(const std::string &path, sound::SoundPlayer &sample_player);
bool loadSoundAssets(sound::SoundPlayer &sample_player);

}  // namespace tetris_clone
