#include "data_encoders/olc_sprite_encoder.hpp"

#include <olcPixelGameEngine.h>

#include <iterator>
#include <memory>
#include <utils/logging.hpp>

namespace data_encoding {

std::unique_ptr<olc::Sprite> createSprite(const std::vector<long>& data) {
  const int width = data.at(0);
  const int height = data.at(1);
  if (data.size() != (width * height) + 2) {
    std::cerr << "[ERROR]: Sprite size does not match meta information"
              << " Width: " << width << " Height: " << height << " Num_Pixels: " << width * height
              << " Actual pixels " << (int)data.size() - 2 << std::endl;
    return nullptr;
  }
  auto sprite_ptr = std::make_unique<olc::Sprite>(width, height);
  int pixel_idx = 0;
  for (auto itr = std::next(data.begin(), 2); itr != data.end(); ++itr) {
    sprite_ptr->GetData()[pixel_idx++] = *itr;
  }
  return sprite_ptr;
}

std::vector<long> spriteToData(const olc::Sprite& sprite, const int max_line_len) {
  std::vector<long> data;
  data.reserve(sprite.height * sprite.width + 2);
  data.push_back(sprite.width);
  data.push_back(sprite.height);
  const auto* p = sprite.GetData();
  for (int i = 0; i < sprite.height * sprite.width; ++i) {
    data.push_back(p[i].n);
  }
  return data;
}

OlcSpriteEncoder::OlcSpriteEncoder() {}

std::any OlcSpriteEncoder::dataToObj(const std::vector<long>& data) const {
  return createSprite(data).release();
}

std::vector<long> OlcSpriteEncoder::objToData(const std::any& object,
                                              const int max_line_len) const {
  olc::Sprite* sprite_ptr;
  try {
    sprite_ptr = std::any_cast<olc::Sprite*>(object);
  } catch (std::bad_any_cast& e) {
    LOG_ERROR("Failed std::any_cast in OlcSpriteEncoder::objToData");
    throw;
  }
  return spriteToData(*sprite_ptr, max_line_len);
}

}  // namespace data_encoding
