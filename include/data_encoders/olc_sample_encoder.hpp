#pragma once

#include <any>
#include <vector>

#include "data_encoders/data_encoder_interface.hpp"

namespace olc{
class AudioSample;
}

class Base64Converter;

namespace data_encoding {

class OlcAudioSampleEncoder : public DataEncoder {
 public:
  OlcAudioSampleEncoder();

  // std::any contains a olc::AudioSample
  std::any dataToObj(const std::vector<long>& data) const override;

  // Expects std::any to be a olc::AudioSample
  std::vector<long> objToData(const std::any& object, const int max_line_len = 80) const override;
};

}  // namespace data_encoding
