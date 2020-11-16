#pragma once

#include <memory>
#include <string>
#include <vector>

namespace olc {
class Sprite;
}

class OlcSpriteEncoder {
 public:
  OlcSpriteEncoder();
  ~OlcSpriteEncoder();

  std::unique_ptr<olc::Sprite> stringToSprite(const std::string& sprite_str);
  std::vector<std::string> spriteToString(const olc::Sprite& sprite, const int max_line_len = 80);

 private:
  std::vector<long> decodeString(const std::string& sprite_encoded, const char token = ',');
  class Base64Converter;
  std::unique_ptr<Base64Converter> base64_conv_;
};
