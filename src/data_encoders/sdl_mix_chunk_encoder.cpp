#include "data_encoders/sdl_mix_chunk_encoder.hpp"

#include <SDL_mixer.h>

#include <iterator>
#include <memory>
#include <utils/logging.hpp>

namespace data_encoding {

std::unique_ptr<Mix_Chunk> createMixChunk(const std::vector<long>& data) {
  auto ret_val = std::make_unique<Mix_Chunk>();
  ret_val->allocated = 1;
  ret_val->alen = data[0];
  ret_val->volume = static_cast<Uint8>(data[1]);
  ret_val->abuf = new Uint8[ret_val->alen];
  auto* payload = ret_val->abuf;
  int i = 0;
  for (auto itr = std::next(data.begin(), 2); itr != data.end(); ++itr) {
    payload[i++] = static_cast<Uint8>(*itr);
  }
  return ret_val;
}

std::vector<long> mixChunkToData(const Mix_Chunk& mix_chunk, const int max_line_len) {
  std::vector<long> ret_val;
  ret_val.reserve(mix_chunk.allocated + 2);
  ret_val.push_back(mix_chunk.alen);
  ret_val.push_back(mix_chunk.volume);
  const auto* data = mix_chunk.abuf;
  for (auto i = 0u; i < mix_chunk.alen; ++i) {
    ret_val.push_back(data[i]);
  }
  return ret_val;
}

SdlMixChunkEncoder::SdlMixChunkEncoder() {}

std::any SdlMixChunkEncoder::dataToObj(const std::vector<long>& data) const {
  return createMixChunk(data).release();
}

std::vector<long> SdlMixChunkEncoder::objToData(const std::any& object,
                                                const int max_line_len) const {
  Mix_Chunk* chunk_ptr;
  try {
    chunk_ptr = std::any_cast<Mix_Chunk*>(object);
  } catch (const std::bad_any_cast) {
    LOG_ERROR("Failed std::any_cast in SdlMixChunkEncoder::objToData");
    throw;
  }
  return mixChunkToData(*chunk_ptr, max_line_len);
}

}  // namespace data_encoding
