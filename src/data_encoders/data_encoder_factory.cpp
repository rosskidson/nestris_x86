#include "data_encoders/data_encoder_factory.hpp"

#include <iostream>
#include <stdexcept>

#include "data_encoders/olc_sprite_encoder.hpp"
#include "data_encoders/sdl_mix_chunk_encoder.hpp"

DataToStringEncoder getDataToStringEncoder(const DataEncoderEnum& encoder) {
  switch (encoder) {
    case (DataEncoderEnum::OlcSprite):
      return DataToStringEncoder{std::make_unique<OlcSpriteEncoder>()};
    case (DataEncoderEnum::SdlMixChunk):
      return DataToStringEncoder{std::make_unique<SdlMixChunkEncoder>()};
  }
  throw std::runtime_error("Missing case in data encoder factory encoder.");
  return DataToStringEncoder{{}};
}
