
#include <SDL_mixer.h>

#include <data_encoders/data_encoder_factory.hpp>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <utils/logging.hpp>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using data_encoding::DataEncoderEnum;

constexpr int MAX_STRING_LENGTH = 7000; // MSVC cannot compile very long strings.

void writeBinaryHeader(const std::string resource_name, const std::string& filename_base) {
  std::ofstream ofs(filename_base + ".hpp");
  ofs << "#pragma once" << std::endl;
  ofs << std::endl;
  ofs << "#include <map>" << std::endl;
  ofs << "#include <string>" << std::endl;
  ofs << "#include <vector>" << std::endl;
  ofs << std::endl;
  ofs << "namespace " << resource_name << " {" << std::endl;
  ofs << "const extern std::map<std::string, std::vector<std::string>> " << resource_name << ";"
      << std::endl;
  ofs << "} // namespace " << resource_name << std::endl;
}

template <typename DataPointer>
void writeBinarySource(const std::string resource_name, const std::string& filename_base,
                       const DataEncoderEnum& data_encoder_type,
                       const std::map<std::string, DataPointer>& data_map) {
  const auto encoder = data_encoding::getDataToStringEncoder(data_encoder_type);
  const std::string indent = "  ";
  std::ofstream ofs(filename_base + ".cpp");
  ofs << "#include \"" << filename_base + ".hpp"
      << "\"";
  ofs << std::endl;
  ofs << std::endl;
  ofs << "namespace " << resource_name << " {" << std::endl;
  ofs << "const std::map<std::string, std::vector<std::string>> " << resource_name << std::endl;
  ofs << indent << "{" << std::endl;

  bool first_element = true;
  for (const auto& [name, data_ptr] : data_map) {
    const auto sprite_text = encoder.objToString(data_ptr.get());
    if (first_element) {
      first_element = false;
    } else {
      ofs << indent << indent << "," << std::endl;
    }
    ofs << indent << indent << "{" << std::endl;
    ofs << indent << indent << indent << "\"" << name << "\"," << std::endl;
    ofs << indent << indent << indent << "{" << std::endl;
    int char_counter = 0;
    for (const auto& line : sprite_text) {
      ofs << indent << indent << indent << indent << "\"" << line << "\"";
      char_counter += line.size();
      if(char_counter > 7000) {
        ofs << "," << std::endl;
        char_counter = 0;
      }
      else {
        ofs << std::endl;
      }
    }
    ofs << indent << indent << indent << indent << "\"\"" << std::endl;
    ofs << indent << indent << indent << "}" << std::endl;
    ofs << indent << indent << "}" << std::endl;
  }

  ofs << indent << "}; // std::map<std::string, std::string> " << resource_name << std::endl;
  ofs << "} // namespace " << resource_name << std::endl;
}

template <typename DataPointer>
void writeBinaryCppFiles(const std::string resource_name, const std::string& filename_base,
                         const DataEncoderEnum& data_encoder_type,
                         const std::map<std::string, DataPointer>& data_map) {
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

std::map<std::string, std::unique_ptr<olc::Sprite>> loadSprites(const std::string& path) {
  namespace fs = std::filesystem;
  std::map<std::string, std::unique_ptr<olc::Sprite>> sprite_map;
  for (const auto& dir_itr : fs::directory_iterator(fs::current_path() / fs::path(path))) {
    const auto filepath = fs::path(dir_itr);
    const auto extension = filepath.extension();
    const auto name = filepath.stem();
    if (extension != ".PNG" and extension != ".png") {
      continue;
    }
    sprite_map[name] = std::make_unique<olc::Sprite>(filepath.string());
    if (not spriteValid(*sprite_map.at(name))) {
      LOG_ERROR("Failed loading `" << filepath << "`.");
      continue;
    }
  }
  return sprite_map;
}

std::map<std::string, std::unique_ptr<Mix_Chunk>> loadSounds(const std::string& path) {
  const int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
  if (result < 0) {
    throw std::runtime_error("Failed to initialize SDL sound mixer.");
  }
  namespace fs = std::filesystem;
  std::map<std::string, std::unique_ptr<Mix_Chunk>> sounds_map;
  for (const auto& dir_itr : fs::directory_iterator(fs::current_path() / fs::path(path))) {
    const auto filepath = fs::path(dir_itr);
    const auto extension = filepath.extension();
    const auto name = filepath.stem();
    if (extension != ".WAV" and extension != ".wav") {
      continue;
    }
    auto raw_ptr = Mix_LoadWAV(filepath.c_str());
    if (raw_ptr == nullptr) {
      LOG_ERROR("Failed loading `" << filepath.string() << "`.");
      continue;
    }
    sounds_map[name] = std::unique_ptr<Mix_Chunk>(raw_ptr);
  }
  return sounds_map;
}

int main() {
  MinimalImpl imp{};
  imp.Construct(10, 10, 8, 8);

  const auto sprite_map = loadSprites("./assets/images");
  writeBinaryCppFiles("images", "images", DataEncoderEnum::OlcSprite, sprite_map);

  const auto sounds_map = loadSounds("./assets/sounds");
  writeBinaryCppFiles("sounds", "sounds", DataEncoderEnum::SdlMixChunk, sounds_map);
}

