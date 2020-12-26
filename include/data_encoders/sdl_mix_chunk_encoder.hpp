#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>

#include "data_encoders/data_encoder_interface.hpp"

class Mix_Chunk;
class Base64Converter;

class SdlMixChunkEncoder : public DataEncoder {
 public:
  SdlMixChunkEncoder();
  ~SdlMixChunkEncoder();

  // std::any contains a Mix_Chunk*
  std::any stringToObj(const std::string& encoded_str) const override;

  // Expects std::any to be a Mix_Chunk*
  std::vector<std::string> objToString(const std::any& object,
                                       const int max_line_len = 80) const override;

 private:
  std::unique_ptr<Mix_Chunk> stringToChunk(const std::string& sprite_str) const;
  std::vector<std::string> chunkToString(const Mix_Chunk& sprite,
                                          const int max_line_len = 80) const;

  //std::vector<long> decodeString(const std::string& sprite_encoded, const char token = ',') const;
  std::unique_ptr<Base64Converter> base64_conv_;
};
