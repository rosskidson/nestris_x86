#include "data_encoders/olc_sprite_encoder.hpp"

#include <olcPixelGameEngine.h>

#include <iostream>
#include <iterator>
#include <memory>
#include <utils/logging.hpp>

namespace data_encoding {

long encodePixel(const olc::Pixel& pixel, const bool encode_alpha) {
  if (encode_alpha) {
    return pixel.n;
  } else {
    return pixel.r << 16 | pixel.g << 8 | pixel.b;
  }
}

olc::Pixel decodePixel(const uint32_t pixel, const bool encode_alpha) {
  if (encode_alpha) {
    return {pixel};
  } else {
    olc::Pixel ret_val;
    ret_val.r = pixel >> 16;
    ret_val.g = (pixel >> 8) & 255;
    ret_val.b = pixel & 255;
    ret_val.a = 255;
    return ret_val;
  }
}

bool detectAlpha(const olc::Sprite& sprite) {
  for (int i = 0; i < sprite.width; ++i) {
    for (int j = 0; j < sprite.height; ++j) {
      if (sprite.GetPixel(i, j).a < 255) {
        return true;
      }
    }
  }
  return false;
}

// Poor man's unit testing.
void testPixelEncoding() {
  {
    olc::Pixel p{0, 0, 0, 255};
    LOG_INFO("### without alpha ###");
    LOG_INFO("p  : " << p.n);
    auto encode = encodePixel(p, false);
    LOG_INFO("enc: " << encode);
    auto decode = decodePixel(encode, false);
    LOG_INFO("dec: " << decode.n);
    LOG_INFO((p.n == decode.n ? "PASS" : "FAIL") << std::endl);
  }

  {
    olc::Pixel p{125, 63, 200, 255};
    LOG_INFO("### without alpha non 0 ###");
    LOG_INFO("p  : " << p.n);
    auto encode = encodePixel(p, false);
    LOG_INFO("enc: " << encode);
    auto decode = decodePixel(encode, false);
    LOG_INFO("dec: " << decode.n);
    LOG_INFO((p.n == decode.n ? "PASS" : "FAIL") << std::endl);
  }

  {
    olc::Pixel p{0, 0, 0, 255};
    LOG_INFO("### with alpha ###");
    LOG_INFO("p  : " << p.n);
    auto encode = encodePixel(p, true);
    LOG_INFO("enc: " << encode);
    auto decode = decodePixel(encode, true);
    LOG_INFO("dec: " << decode.n);
    LOG_INFO((p.n == decode.n ? "PASS" : "FAIL") << std::endl);
  }

  {
    olc::Pixel p{125, 63, 200, 255};
    LOG_INFO("### with alpha non 0 ###");
    LOG_INFO("p  : " << p.n);
    auto encode = encodePixel(p, true);
    LOG_INFO("enc: " << encode);
    auto decode = decodePixel(encode, true);
    LOG_INFO("dec: " << decode.n);
    LOG_INFO((p.n == decode.n ? "PASS" : "FAIL") << std::endl);
  }
}

std::unique_ptr<olc::Sprite> createSprite(const std::vector<long>& data) {
  const int width = data.at(0);
  const int height = data.at(1);
  const bool alpha_encoded = data.at(2) > 0;
  const int extra_fields = 3;
  if (static_cast<int>(data.size()) != (width * height) + extra_fields) {
    std::cerr << "[ERROR]: Sprite size does not match meta information"
              << " Width: " << width << " Height: " << height << " Num_Pixels: " << width * height
              << " Actual pixels " << (int)data.size() - extra_fields << std::endl;
    return nullptr;
  }
  auto sprite_ptr = std::make_unique<olc::Sprite>(width, height);
  int pixel_idx = 0;
  for (auto itr = std::next(data.begin(), extra_fields); itr != data.end(); ++itr) {
    sprite_ptr->GetData()[pixel_idx++] = decodePixel(*itr, alpha_encoded);
  }
  return sprite_ptr;
}

std::vector<long> spriteToData(const olc::Sprite& sprite, const int max_line_len) {
  std::vector<long> data;
  const bool encode_alpha = detectAlpha(sprite);
  data.reserve(sprite.height * sprite.width + 2);
  data.push_back(sprite.width);
  data.push_back(sprite.height);
  data.push_back(encode_alpha);
  const auto* p = sprite.GetData();
  for (int i = 0; i < sprite.height * sprite.width; ++i) {
    data.push_back(encodePixel(p[i], encode_alpha));
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
  } catch (std::bad_any_cast&) {
    LOG_ERROR("Failed std::any_cast in OlcSpriteEncoder::objToData");
    throw;
  }
  return spriteToData(*sprite_ptr, max_line_len);
}

}  // namespace data_encoding
