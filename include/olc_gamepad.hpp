#pragma once

#include <map>
#include <vector>

#include "input_interface.hpp"
#include "olcPixelGameEngine.h"

namespace tetris_clone {

class OlcGamePad : public InputInterface {
 public:
  OlcGamePad();
  ~OlcGamePad();
  bool detectAndInit();

  bool getKeyState(const KeyCode key_code) const override;
  KeyCode getPressedKey() const override;

  std::string keyCodeToStr(const KeyCode key_code) const override;
  KeyCode lookupKeyCode(const std::string& key_name) const override;
  KeyCode getNullKey() const override;

 private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;
};

}  // namespace tetris_clone
