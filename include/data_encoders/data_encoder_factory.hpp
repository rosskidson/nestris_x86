#pragma once

#include "data_to_string_encoder.hpp"

enum class DataEncoderEnum { OlcSprite, SdlMixChunk };

DataToStringEncoder getDataToStringEncoder(const DataEncoderEnum& encoder);
