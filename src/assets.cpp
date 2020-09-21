
#include "assets.hpp"
#include "logging.hpp"
#include <sstream>

#include "olcPixelGameEngine.h"

namespace tetris_clone {

namespace {
bool spriteValid(const olc::Sprite &sprite) { return sprite.height > 0 && sprite.width > 0; }
} // namespace

// clang-format off
const std::vector<std::pair<std::string, std::string>> SAMPLES{
    {"tetromino_move.wav", "move"},
    {"tetromino_lock.wav", "lock"},
    {"tetromino_rotate.wav", "rotate"},
    {"tetris.wav", "tetris"},
    {"line_clear.wav", "line_clear"},
    {"menu_blip.wav", "menu_blip"},
    {"menu_select_01.wav", "menu_select_01"},
    {"menu_select_02.wav", "menu_select_02"},
    {"pause.wav", "pause"},
    {"level_up.wav", "level_up"},
    {"top_out.wav", "top_out"}};

const std::vector<std::pair<std::string, std::string>> SPRITES{
    {"basic_field_empty.png", "background"},
    {"basic_field_flash.png", "background_flash"},
    {"level-screen.png", "menu"},
    {"l0-counts.png", "l0-counts"},
    {"l1-counts.png", "l1-counts"},
    {"l2-counts.png", "l2-counts"},
    {"l3-counts.png", "l3-counts"},
    {"l4-counts.png", "l4-counts"},
    {"l5-counts.png", "l5-counts"},
    {"l6-counts.png", "l6-counts"},
    {"l7-counts.png", "l7-counts"},
    {"l8-counts.png", "l8-counts"},
    {"l9-counts.png", "l9-counts"}};
// clang-format on

bool loadBlockSprites(const std::string &path,
                      std::vector<std::vector<std::unique_ptr<olc::Sprite>>> &block_sprites) {
  block_sprites.clear();
  block_sprites.resize(10);
  for (int level = 0; level < 10; ++level) {
    block_sprites[level].emplace_back(std::make_unique<olc::Sprite>(8, 8));
    for (int color = 0; color < 4; ++color) {
      std::stringstream ss;
      ss << "/l" << level << "-c" << color << ".png";
      const std::string full_path = path + ss.str();
      block_sprites[level].emplace_back(std::make_unique<olc::Sprite>(full_path));
      if (not spriteValid(*block_sprites.at(level).back())) {
        LOG_ERROR("Failed loading `" << full_path << "`.");
        return false;
      }
    }
  }
  return true;
}

bool loadSprites(const std::string &path,
                 std::map<std::string, std::unique_ptr<olc::Sprite>> &sprite_map) {
  for (const auto &[filename, name] : SPRITES) {
    const std::string full_path = path + "/" + filename;
    sprite_map[name] = std::make_unique<olc::Sprite>(full_path);
    if (not spriteValid(*sprite_map.at(name))) {
      LOG_ERROR("Failed loading sprite `" << full_path << "`");
      return false;
    }
  }
  return true;
}

bool loadSoundAssets(const std::string &path, sound::SoundPlayer &sample_player) {
  bool ret = true;
  for (const auto &[filename, name] : SAMPLES) {
    ret = ret && sample_player.loadWavFromFilesystem(path + filename, name);
  }

  return ret;
}

} // namespace tetris_clone
