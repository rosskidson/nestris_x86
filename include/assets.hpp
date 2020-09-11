
#include "sound.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace olc {
class Sprite;
}

namespace tetris_clone {

bool loadBlockSprites(const std::string &path,
                      std::vector<std::vector<std::unique_ptr<olc::Sprite>>> &block_sprites);

bool loadSprites(const std::string &path,
                 std::map<std::string, std::unique_ptr<olc::Sprite>> &sprite_map);

bool loadSoundAssets(const std::string &path, sound::SoundPlayer &sample_player);

} // namespace tetris_clone
