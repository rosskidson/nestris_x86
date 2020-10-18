#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "sound.hpp"

namespace olc {
class Sprite;
}

namespace tetris_clone {

class SpriteProvider {
 public:
  SpriteProvider(const std::string& path);

  bool loadSprites(const std::string &path);

  olc::Sprite *getSprite(const std::string &sprite_name) const;

 private:
  std::map<std::string, std::unique_ptr<olc::Sprite>> sprite_map_;
};

bool loadSoundAssets(const std::string &path, sound::SoundPlayer &sample_player);

}  // namespace tetris_clone
