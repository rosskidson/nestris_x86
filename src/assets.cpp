
#include "assets.hpp"

#include <SDL_mixer.h>

#include <filesystem>
#include <sstream>
#include <stdexcept>

#include "assets_cpp/images.hpp"
#include "assets_cpp/sounds.hpp"
#include "data_encoders/data_encoder_factory.hpp"
#include "utils/logging.hpp"

#include "Extensions/olcPGEX_Sound.h"

namespace nestris_x86 {

namespace fs = std::filesystem;
using data_encoding::DataEncoderEnum;

namespace {
bool spriteValid(const olc::Sprite *sprite) {
  if (sprite == nullptr) {
    return false;
  }
  return sprite->height > 0 && sprite->width > 0;
}
}  // namespace

// clang-format off
const std::vector<std::pair<std::string, std::string>> SAMPLES{
    {"tetromino_move.wav", "tetromino_move"},
    {"tetromino_lock.wav", "tetromino_lock"},
    {"tetromino_rotate.wav", "tetromino_rotate"},
    {"tetris.wav", "tetris"},
    {"line_clear.wav", "line_clear"},
    {"menu_blip.wav", "menu_blip"},
    {"menu_select_01.wav", "menu_select_01"},
    {"menu_select_02.wav", "menu_select_02"},
    {"pause.wav", "pause"},
    {"level_up.wav", "level_up"},
    {"top_out.wav", "top_out"}};

// clang-format on

SpriteProvider::SpriteProvider(const std::string &path) {
  if (not loadSprites(path)) {
    throw std::runtime_error("Failed initializing SpriteProvider with path `" + path + "`");
  }
}

SpriteProvider::SpriteProvider() {
  if (LOAD_FROM_BINARY) {
    if (not loadSprites()) {
      throw std::runtime_error("Failed initializing SpriteProvider from header.");
    }
  } else {
    if (not loadSprites("./assets/images/")) {
      throw std::runtime_error("Failed initializing SpriteProvider from header.");
    }
  }
}

olc::Sprite *SpriteProvider::getSprite(const std::string &sprite_name) const try {
  return sprite_map_.at(sprite_name).get();
} catch (const std::out_of_range &e) {
  LOG_ERROR("Sprite not loaded. Sprite name: `" << sprite_name << "`");
  throw;
}

bool SpriteProvider::loadSprites(const std::string &path) {
  for (const auto &dir_itr : fs::directory_iterator(fs::current_path() / fs::path(path))) {
    const auto filepath = fs::path(dir_itr);
    const auto extension = filepath.extension();
    const auto name = filepath.stem();
    if (extension != ".PNG" and extension != ".png") {
      continue;
    }
    sprite_map_[name] = std::make_unique<olc::Sprite>(filepath.string());
    if (not spriteValid(sprite_map_.at(name).get())) {
      LOG_ERROR("Failed loading `" << filepath << "`.");
      return false;
    }
  }
  return true;
}

bool SpriteProvider::loadSprites() {
  LOG_INFO("Loading sprites...");
  const auto decoder = data_encoding::getDataToStringEncoder(DataEncoderEnum::OlcSprite);
  for (const auto &[id, code] : images::images) {
    const auto raw_ptr = std::any_cast<olc::Sprite *>(decoder.stringToObj(code));
    sprite_map_[id] = std::unique_ptr<olc::Sprite>{raw_ptr};
    if (not spriteValid(sprite_map_.at(id).get())) {
      LOG_ERROR("Failed loading `" << id << "`.");
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

bool loadSoundAssets(sound::SoundPlayer &sample_player) {
  LOG_INFO("Loading sounds...");
  const auto decoder = data_encoding::getDataToStringEncoder(DataEncoderEnum::OlcAudioSample);
  for (const auto &[id, code] : sounds::sounds) {
    const auto sample = std::any_cast<olc::AudioSample>(decoder.stringToObj(code));
    sample_player.loadWavFromMemory(sample, id);
  }
  return true;
}

}  // namespace nestris_x86
