#include "data_encoders/data_to_string_encoder.hpp"

#include <iostream>
#include <memory>
#include <sstream>

#include "utils/base64_converter.hpp"

DataToStringEncoder::DataToStringEncoder(std::unique_ptr<DataEncoder>&& data_encoder)
    : data_encoder_{std::move(data_encoder)}, base64_conv_(std::make_unique<Base64Converter>()) {}

DataToStringEncoder::~DataToStringEncoder() = default;

std::any DataToStringEncoder::stringToObj(const std::string& encoded_str) const {
  return data_encoder_->dataToObj(decodeString(encoded_str));
}

std::vector<std::string> DataToStringEncoder::objToString(const std::any& object,
                                                          const int max_line_len) const {
  return encodeString(data_encoder_->objToData(object), max_line_len);
}

std::vector<std::string> DataToStringEncoder::encodeString(const std::vector<long>& data,
                                                           const int max_line_len) const {
  std::stringstream stream;
  std::vector<std::string> return_val;
  for (const auto& num : data) {
    stream << base64_conv_->encodeNumber(num) << ",";
    if (stream.str().length() > max_line_len) {
      return_val.push_back(stream.str());
      stream.str("");
    }
  }
  return return_val;
}

std::vector<long> DataToStringEncoder::decodeString(const std::string& sprite_encoded,
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
