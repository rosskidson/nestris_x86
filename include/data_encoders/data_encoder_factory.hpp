#pragma once

#include "data_to_string_encoder.hpp"

namespace data_encoding {
enum class DataEncoderEnum { OlcSprite, OlcAudioSample, SdlMixChunk };

DataToStringEncoder getDataToStringEncoder(const DataEncoderEnum& encoder);

}  // namespace data_encoding
