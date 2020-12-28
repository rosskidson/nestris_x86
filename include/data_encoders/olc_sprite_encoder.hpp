#pragma once

#include <any>
#include <vector>

#include "data_encoders/data_encoder_interface.hpp"

namespace data_encoding {

class OlcSpriteEncoder : public DataEncoder {
 public:
  OlcSpriteEncoder();

  // std::any contains an olc::Sprite*
  std::any dataToObj(const std::vector<long>& data) const override;

  // Expects std::any to be an olc::Sprite*
  std::vector<long> objToData(const std::any& object, const int max_line_len = 80) const override;
};

}
