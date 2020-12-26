#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "data_encoders/data_encoder_interface.hpp"

namespace olc {
class Sprite;
}
class Base64Converter;

class OlcSpriteEncoder : public DataEncoder {
 public:
  OlcSpriteEncoder();
  ~OlcSpriteEncoder();

  // std::any contains an olc::Sprite*
  std::any stringToObj(const std::string& encoded_str) const override;

  // Expects std::any to be an olc::Sprite*
  std::vector<std::string> objToString(const std::any& object,
                                       const int max_line_len = 80) const override;

  std::unique_ptr<olc::Sprite> stringToSprite(const std::string& sprite_str) const;
  std::vector<std::string> spriteToString(const olc::Sprite& sprite,
                                          const int max_line_len = 80) const;

 private: // TODO:: make above two functions private, change usage to objToString.
  std::vector<long> decodeString(const std::string& sprite_encoded, const char token = ',') const;
  std::unique_ptr<Base64Converter> base64_conv_;
};
