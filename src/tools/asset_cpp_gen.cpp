
#include <data_encoders/data_encoder_factory.hpp>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <utils/logging.hpp>

// clang-format off
#include "olcPixelGameEngine.h"
#include "Extensions/olcPGEX_Sound.h"
// clang-format on

using data_encoding::DataEncoderEnum;

void writeBinaryHeader(const std::string resource_name, const std::string& filename_base) {
  std::ofstream ofs(filename_base + ".hpp");
  ofs << "#pragma once" << std::endl;
  ofs << std::endl;
  ofs << "#include <map>" << std::endl;
  ofs << "#include <string>" << std::endl;
  ofs << std::endl;
  ofs << "namespace " << resource_name << " {" << std::endl;
  ofs << "const extern std::map<std::string, std::string> " << resource_name << ";" << std::endl;
  ofs << "} // namespace " << resource_name << std::endl;
}

template <typename DataType>
void writeBinarySource(const std::string resource_name, const std::string& filename_base,
                       const DataEncoderEnum& data_encoder_type,
                       const std::map<std::string, DataType>& data_map) {
  const auto encoder = data_encoding::getDataToStringEncoder(data_encoder_type);
  const std::string indent = "  ";
  std::ofstream ofs(filename_base + ".cpp");
  ofs << "#include \"" << filename_base + ".hpp"
      << "\"";
  ofs << std::endl;
  ofs << std::endl;
  ofs << "namespace " << resource_name << " {" << std::endl;
  ofs << "const std::map<std::string, std::string> " << resource_name << std::endl;
  ofs << indent << "{" << std::endl;

  bool first_element = true;
  for (const auto& [name, data] : data_map) {
    const auto sprite_text = encoder.objToString(data);
    if (first_element) {
      first_element = false;
    } else {
      ofs << indent << indent << "," << std::endl;
    }
    ofs << indent << indent << "{\"" << name << "\"," << std::endl;
    for (const auto& line : sprite_text) {
      ofs << indent << indent << " \"" << line << "\"" << std::endl;
    }
    ofs << indent << indent << "}" << std::endl;
  }

  ofs << indent << "}; // std::map<std::string, std::string> " << resource_name << std::endl;
  ofs << "} // namespace " << resource_name << std::endl;
}

template <typename DataType>
void writeBinaryCppFiles(const std::string resource_name, const std::string& filename_base,
                         const DataEncoderEnum& data_encoder_type,
                         const std::map<std::string, DataType>& data_map) {
  writeBinaryHeader(resource_name, filename_base);
  writeBinarySource(resource_name, filename_base, data_encoder_type, data_map);
}

class MinimalImpl : public olc::PixelGameEngine {
 public:
  bool OnUserCreate() override { return true; }
  bool OnUserUpdate(float fElapsedTime) override { return true; }
};

bool spriteValid(const olc::Sprite& sprite) {
  return sprite.height > 0 && sprite.width > 0;
}

std::map<std::string, olc::Sprite*> loadSprites(const std::string& path) {
  namespace fs = std::filesystem;
  std::map<std::string, olc::Sprite*> sprite_map;
  for (const auto& dir_itr : fs::directory_iterator(fs::current_path() / fs::path(path))) {
    const auto filepath = fs::path(dir_itr);
    const auto extension = filepath.extension();
    const auto name = filepath.stem();
    if (extension != ".PNG" and extension != ".png") {
      continue;
    }
    sprite_map[name] = new olc::Sprite(filepath.string());
    if (not spriteValid(*sprite_map.at(name))) {
      LOG_ERROR("Failed loading `" << filepath << "`.");
      continue;
    }
  }
  return sprite_map;
}

std::map<std::string, olc::AudioSample> loadSounds(const std::string& path) {
  namespace fs = std::filesystem;
  std::map<std::string, olc::AudioSample> sounds_map;
  for (const auto& dir_itr : fs::directory_iterator(fs::current_path() / fs::path(path))) {
    const auto filepath = fs::path(dir_itr);
    const auto extension = filepath.extension();
    const auto name = filepath.stem();
    if (extension != ".WAV" and extension != ".wav") {
      continue;
    }
    olc::AudioSample sample;
    bool success = sample.LoadFromFile(filepath.c_str());
    if (not success || not sample.bSampleValid) {
      LOG_ERROR("Failed loading `" << filepath.string() << "`.");
      continue;
    }
    sounds_map[name] = sample;
  }
  return sounds_map;
}

int main() {
  MinimalImpl imp{};
  imp.Construct(10, 10, 8, 8);

  //const auto sprite_map = loadSprites("./assets/images");
  //writeBinaryCppFiles("images", "images", DataEncoderEnum::OlcSprite, sprite_map);

  const auto sounds_map = loadSounds("./assets/sounds");
  writeBinaryCppFiles("sounds", "sounds", DataEncoderEnum::OlcAudioSample, sounds_map);
}

