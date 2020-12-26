#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "data_encoders/data_encoder_interface.hpp"

class Base64Converter;

class DataToStringEncoder {
 public:
  DataToStringEncoder(std::unique_ptr<DataEncoder>&& data_encoder);
  ~DataToStringEncoder();

  // std::any contains the type expected by data_encoder_
  std::any stringToObj(const std::string& encoded_str) const;

  // Expects std::any to be the one intended for data_encoder_
  std::vector<std::string> objToString(const std::any& object, const int max_line_len = 80) const;

 private:
  std::vector<std::string> encodeString(const std::vector<long>& data,
                                        const int max_line_len) const;
  std::vector<long> decodeString(const std::string& sprite_encoded, const char token = ',') const;

  std::unique_ptr<DataEncoder> data_encoder_;
  std::unique_ptr<Base64Converter> base64_conv_;
};

