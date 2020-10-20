#pragma once

#include <map>
#include <vector>

#include "input_interface.hpp"
#include "olcPixelGameEngine.h"

namespace tetris_clone {

class OlcKeyboard : public InputInterface {
 public:
  OlcKeyboard(olc::PixelGameEngine& pixel_game_engine_ref);

  bool getKeyState(const KeyCode key_code) const override;

  KeyCode getPressedKey() const override;

  std::string keyCodeToStr(const KeyCode key_code) const override;
  KeyCode lookupKeyCode(const std::string& key_name) const override;

 private:
  static KeyCode olcToKeyCode(const olc::Key& key);
  static olc::Key keyCodeToOlc(const KeyCode key_code);

  olc::PixelGameEngine& pixel_game_engine_ref_;
  static constexpr int num_keys{85};
  const static std::map<std::string, int> name_code_lookup;
  const static std::array<std::string, 85> key_names;
};

}  // namespace tetris_clone
