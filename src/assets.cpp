
#include "assets.hpp"

#include <SDL_mixer.h>
#include <iso646.h>

#include <filesystem>
#include <sstream>
#include <stdexcept>

#include "assets_cpp/images.hpp"
#include "assets_cpp/sounds.hpp"
#include "data_encoders/data_encoder_factory.hpp"
#include "utils/logging.hpp"

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
    sprite_map_[name.string()] = std::make_unique<olc::Sprite>(filepath.string());
    if (not spriteValid(sprite_map_.at(name.string()).get())) {
      LOG_ERROR("Failed loading `" << filepath << "`.");
      return false;
    }
  }
  return true;
}

std::string concatenateLines(const std::vector<std::string> &lines) {
  std::string ret_val;
  for (const auto &line : lines) {
    ret_val += line;
  }
  return ret_val;
}

bool SpriteProvider::loadSprites() {
  LOG_INFO("Loading sprites...");
  const auto decoder = data_encoding::getDataToStringEncoder(DataEncoderEnum::OlcSprite);
  for (const auto &[id, lines] : images::images) {
    const auto raw_ptr = std::any_cast<olc::Sprite *>(decoder.stringToObj(concatenateLines(lines)));
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
  for (const auto &dir_itr : fs::directory_iterator(fs::current_path() / fs::path(path))) {
    const auto filepath = fs::path(dir_itr);
    const auto extension = filepath.extension();
    const auto name = filepath.stem();
    if (extension != ".WAV" and extension != ".wav") {
      continue;
    }
    ret = ret && sample_player.loadWavFromFilesystem(filepath.string(), name);
  }

  return ret;
}

bool loadSoundAssets(sound::SoundPlayer &sample_player) {
  LOG_INFO("Loading sounds...");
  const auto decoder = data_encoding::getDataToStringEncoder(DataEncoderEnum::SdlMixChunk);
  for (const auto &[id, line] : sounds::sounds) {
    const auto raw_ptr = std::any_cast<Mix_Chunk *>(decoder.stringToObj(concatenateLines(line)));
    const auto success = sample_player.loadWavFromMemory(std::unique_ptr<Mix_Chunk>(raw_ptr), id);
    if (not success) {
      LOG_ERROR("Failed loading sound `" << id << "`");
      return false;
    }
  }
  return true;
}

}  // namespace nestris_x86
