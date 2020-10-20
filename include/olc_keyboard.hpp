#pragma once

#include <map>
#include <vector>

#include "input_interface.hpp"
#include "olcPixelGameEngine.h"

namespace tetris_clone {

class OlcKeyboard : public InputInterface {
 public:
  OlcKeyboard(olc::PixelGameEngine& pixel_game_engine_ref);

  bool getKeyState(const int key_code) const override;

  int getPressedKey() const override;

  std::string keyCodeToStr(const int key_code) const override;
  int lookupKeyCode(const std::string& key_name) const override;

 private:
  int olcToKeyCode(const olc::Key& key) const;
  olc::Key keyCodeToOlc(const int& key_code) const;

  olc::PixelGameEngine& pixel_game_engine_ref_;
  static constexpr int num_keys{85};
};

}  // namespace tetris_clone
