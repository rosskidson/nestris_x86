#include "data_encoders/olc_sample_encoder.hpp"

#include "Extensions/olcPGEX_Sound.h"

#include <iterator>

#include "utils/logging.hpp"

namespace data_encoding {

olc::AudioSample createSampleFromData(const std::vector<long>& data) {
  olc::AudioSample ret_val;
  ret_val.wavHeader.wFormatTag = data[0];
  ret_val.wavHeader.nChannels = data[1];
  ret_val.wavHeader.nSamplesPerSec = data[2];
  ret_val.wavHeader.nAvgBytesPerSec = data[3];
  ret_val.wavHeader.nBlockAlign = data[4];
  ret_val.wavHeader.wBitsPerSample = data[5];
  ret_val.wavHeader.cbSize = data[6];
  ret_val.nSamples = data[7];
  ret_val.nChannels = data[8];
  ret_val.bSampleValid = data[9];
  ret_val.fSample = new float[ret_val.nSamples * ret_val.nChannels];
  long i = 0;
  auto* payload = ret_val.fSample;
  for (auto itr = std::next(data.begin(), 10); itr != data.end(); ++itr) {
    payload[i++] = *itr / 1e7;
  }

  return ret_val;
}

std::vector<long> audioSampleToData(const olc::AudioSample& sample, const int max_line_len) {
  std::vector<long> ret_val;
  ret_val.push_back(sample.wavHeader.wFormatTag);
  ret_val.push_back(sample.wavHeader.nChannels);
  ret_val.push_back(sample.wavHeader.nSamplesPerSec);
  ret_val.push_back(sample.wavHeader.nAvgBytesPerSec);
  ret_val.push_back(sample.wavHeader.nBlockAlign);
  ret_val.push_back(sample.wavHeader.wBitsPerSample);
  ret_val.push_back(sample.wavHeader.cbSize);
  ret_val.push_back(sample.nSamples);
  ret_val.push_back(sample.nChannels);
  ret_val.push_back(sample.bSampleValid);
  for (long i = 0; i < sample.nSamples * sample.nChannels; ++i) {
    ret_val.push_back(sample.fSample[i] * 1e7);
  }
  return ret_val;
}

OlcAudioSampleEncoder::OlcAudioSampleEncoder() {}

std::any OlcAudioSampleEncoder::dataToObj(const std::vector<long>& data) const {
  return createSampleFromData(data);
}

std::vector<long> OlcAudioSampleEncoder::objToData(const std::any& object,
                                                   const int max_line_len) const {
  olc::AudioSample sample;
  try {
    sample = std::any_cast<olc::AudioSample>(object);
  } catch (const std::bad_any_cast& e) {
    LOG_ERROR("Failed std::any_cast in OlcAudioSampleEncoder::objToData");
    throw;
  }
  return audioSampleToData(sample, max_line_len);
}

}  // namespace data_encoding
