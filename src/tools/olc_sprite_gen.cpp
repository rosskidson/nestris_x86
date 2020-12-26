
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <data_encoders/olc_sprite_encoder.hpp>
#include <string>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

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

void writeBinarySource(const std::string resource_name, const std::string& filename_base,
                           const std::map<std::string, std::unique_ptr<olc::Sprite>>& sprites) {
  OlcSpriteEncoder sprite_encoder{};
  const std::string indent = "  ";
  std::ofstream ofs(filename_base + ".cpp");
  ofs << "#include \"" << filename_base + ".hpp" << "\"";
  ofs << std::endl;
  ofs << std::endl;
  ofs << "namespace " << resource_name << " {" << std::endl;
  ofs << "const std::map<std::string, std::string> " << resource_name << std::endl;
  ofs << indent << "{" << std::endl;

  bool first_element = true;
  for (const auto& [name, sprite] : sprites) {
    const auto sprite_text = sprite_encoder.spriteToString(*sprite);
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

void writeBinaryCppFiles(const std::string resource_name, const std::string& filename_base,
                           const std::map<std::string, std::unique_ptr<olc::Sprite>>& sprites) {
  writeBinaryHeader(resource_name, filename_base);
  writeBinarySource(resource_name, filename_base, sprites);
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
      std::cerr << "Failed loading `" << filepath << "`." << std::endl;
      continue;
    }
  }
  return sprite_map;
}

int main() {
  MinimalImpl imp{};
  imp.Construct(10, 10, 8, 8);
  // olc::Sprite test_sprite("assets/images/levels-screen.png");
  // OlcSpriteEncoder encoder;
  // encoder.spriteToString(test_sprite);
  // if (not test_sprite.GetData()) {
  //  std::cout << "Failed loading sprite." << std::endl;
  //  return -1;
  //}

  const auto sprite_map = loadSprites("./assets/images");
  writeBinaryCppFiles("images", "images", sprite_map);
}

