#pragma once

#include <any>
#include <vector>

#include "data_encoders/data_encoder_interface.hpp"

class Mix_Chunk;
class Base64Converter;

class SdlMixChunkEncoder : public DataEncoder {
 public:
  SdlMixChunkEncoder();

  // std::any contains a Mix_Chunk*
  std::any dataToObj(const std::vector<long>& data) const override;

  // Expects std::any to be a Mix_Chunk*
  std::vector<long> objToData(const std::any& object, const int max_line_len = 80) const override;

};
