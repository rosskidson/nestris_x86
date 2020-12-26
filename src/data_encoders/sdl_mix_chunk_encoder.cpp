#include "data_encoders/sdl_mix_chunk_encoder.hpp"

#include <SDL_mixer.h>

#include <deque>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>

#include "utils/base64_converter.hpp"

SdlMixChunkEncoder::SdlMixChunkEncoder() : base64_conv_(std::make_unique<Base64Converter>()) {}
SdlMixChunkEncoder::~SdlMixChunkEncoder() = default;

std::any SdlMixChunkEncoder::stringToObj(const std::string& encoded_str) const {
  return stringToChunk(encoded_str).release();
}

std::vector<std::string> SdlMixChunkEncoder::objToString(const std::any& object,
                                                       const int max_line_len) const {
  const auto* sprite = std::any_cast<Mix_Chunk*>(object);
  return chunkToString(*sprite, max_line_len);
}

std::vector<long> OlcSpriteEncoder::decodeString(const std::string& sprite_encoded,
                                                 const char token) const {
  std::vector<long> return_value;
  std::string current_word{};
  for (const auto c : sprite_encoded) {
    if (c == token) {
      if (current_word.empty()) {
        std::cerr << "[WARNING]: Empty b64 word" << std::endl;
        continue;
      }
      return_value.push_back(base64_conv_->decodeNumber(current_word));
      current_word.clear();
    } else {
      current_word.push_back(c);
    }
  }
  if (not current_word.empty()) {
    return_value.push_back(base64_conv_->decodeNumber(current_word));
  }
  return return_value;
}

std::unique_ptr<olc::Sprite> OlcSpriteEncoder::stringToSprite(const std::string& sprite_str) const {
  const auto tokenize_data = decodeString(sprite_str);
  return createSprite(tokenize_data);
}

std::vector<std::string> OlcSpriteEncoder::spriteToString(const olc::Sprite& sprite,
                                                          const int max_line_len) const {
  std::stringstream stream;
  std::vector<std::string> return_val;
  stream << base64_conv_->encodeNumber(sprite.width) << ",";
  stream << base64_conv_->encodeNumber(sprite.height) << ",";
  const auto* p = sprite.GetData();
  for (int i = 0; i < sprite.height * sprite.width; ++i) {
    stream << base64_conv_->encodeNumber(p[i].n) << ",";
    if (stream.str().length() > max_line_len) {
      return_val.push_back(stream.str());
      stream.str("");
    }
  }
  return_val.push_back(stream.str());
  return return_val;
}
