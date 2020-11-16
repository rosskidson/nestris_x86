#include "olc_sprite_encoder.hpp"

#include <olcPixelGameEngine.h>

#include <iterator>
#include <sstream>
#include <deque>
#include <iostream>
#include <map>
#include <memory>

class OlcSpriteEncoder::Base64Converter {
 public:
  Base64Converter()
      : base_64_chars_{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"},
        char_to_num_{} {
    int idx = 0;
    for (const auto& character : base_64_chars_) {
      char_to_num_[character] = idx++;
    }
  }

  std::string encodeNumber(const long num) {
    if(num == 0) {
      return std::string(1, base_64_chars_[0]);
    }
    auto tmp_num = num;
    std::deque<char> encoded_num;
    while (tmp_num > 0) {
      encoded_num.push_front(base_64_chars_[tmp_num % 64]);
      tmp_num /= 64;
    }
    return {encoded_num.begin(), encoded_num.end()};
  }

  long decodeNumber(const std::string& code) {
    long return_value = 0;
    for (const auto character : code) {
      return_value *= 64;
      return_value += char_to_num_.at(character);
    }
    return return_value;
  }

 private:
  std::string base_64_chars_;
  std::map<char, int> char_to_num_;
};

std::unique_ptr<olc::Sprite> createSprite(const std::vector<long>& data) {
  const int width = data.at(0);
  const int height = data.at(1);
  if (data.size() != (width * height) + 2) {
    std::cerr << "[ERROR]: Sprite size does not match meta information"
              << "Width: " << width << " Height: " << height << " Num_Pixels: " << width * height
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

OlcSpriteEncoder::OlcSpriteEncoder() : base64_conv_(std::make_unique<Base64Converter>()) {}
OlcSpriteEncoder::~OlcSpriteEncoder() = default;

std::vector<long> OlcSpriteEncoder::decodeString(const std::string& sprite_encoded,
                                                 const char token) {
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

std::unique_ptr<olc::Sprite> OlcSpriteEncoder::stringToSprite(const std::string& sprite_str) {
  const auto tokenize_data = decodeString(sprite_str);
  return createSprite(tokenize_data);
}

std::vector<std::string> OlcSpriteEncoder::spriteToString(const olc::Sprite& sprite,
                                                          const int max_line_len) {
  std::stringstream stream;
  std::vector<std::string> return_val;
  Base64Converter converter{};
  stream << converter.encodeNumber(sprite.width) << ",";
  stream << converter.encodeNumber(sprite.height) << ",";
  const auto* p = sprite.GetData();
  for (int i = 0; i < sprite.height * sprite.width; ++i) {
    stream << converter.encodeNumber(p[i].n) << ",";
    if (stream.str().length() > max_line_len) {
      return_val.push_back(stream.str());
      stream.str("");
    }
  }
  return_val.push_back(stream.str());
  return return_val;
}
