#include "data_encoders/data_encoder_factory.hpp"

#include "data_encoders/olc_sprite_encoder.hpp"
//#include "data_encoders/sdl_mix_chunk_encoder.hpp"



#include <iostream>

DataToStringEncoder getDataToStringEncoder(const DataEncoderEnum& encoder) {
  switch (encoder) {
    case (DataEncoderEnum::OlcSprite):
      return DataToStringEncoder{std::make_unique<OlcSpriteEncoder>()};
  }
  std::cout << "swtich statement failed." << std::endl;
}
