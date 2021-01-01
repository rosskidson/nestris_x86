#pragma once

#include <deque>
#include <map>

class Base64Converter {
 public:
  inline Base64Converter()
      : base_64_chars_{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789$_"},
        char_to_num_{} {
    int idx = 0;
    for (const auto& character : base_64_chars_) {
      char_to_num_[character] = idx++;
    }
  }

  inline std::string encodeNumber(const long num) const {
    if (num == 0) {
      return std::string(1, base_64_chars_[0]);
    }
    auto tmp_num = num > 0 ? num : -num;
    std::deque<char> encoded_num;
    while (tmp_num > 0) {
      encoded_num.push_front(base_64_chars_[tmp_num % 64]);
      tmp_num /= 64;
    }
    if (num < 0) {
      encoded_num.push_front('-');
    }
    return {encoded_num.begin(), encoded_num.end()};
  }

  inline long decodeNumber(const std::string& code) const {
    long return_value = 0;
    int sign_multiplier = 1;
    for (const auto character : code) {
      if (character == '-') {
        sign_multiplier = -1;
        continue;
      }
      return_value *= 64;
      return_value += char_to_num_.at(character);
    }
    return sign_multiplier * return_value;
  }

 private:
  std::string base_64_chars_;
  std::map<char, int> char_to_num_;
};

