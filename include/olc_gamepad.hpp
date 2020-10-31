#pragma once

#include <map>
#include <vector>

#include "Contributions/olcPGEX_Gamepad.h"
#include "input_interface.hpp"
#include "olcPixelGameEngine.h"

namespace tetris_clone {

class OlcGamePad : public InputInterface {
 public:
  OlcGamePad();

  bool detectAndInit();

  bool getKeyState(const KeyCode key_code) const override;

  KeyCode getPressedKey() const override;

  std::string keyCodeToStr(const KeyCode key_code) const override;
  KeyCode lookupKeyCode(const std::string& key_name) const override;

  KeyCode getNullKey() const override;

 private:
  static KeyCode olcToKeyCode(const olc::GPButtons& key);
  static olc::GPButtons keyCodeToOlc(const KeyCode key_code);

  std::vector<olc::GamePad> gamepads_;
  // HACKS:: The pointer indirection is because GamePad::poll() and GamePad::getButton are not const
  olc::GamePad gamepad_;
  olc::GamePad* gamepad_ptr_;
  const static std::map<std::string, int> name_to_code_;
  const static std::map<int, std::string> code_to_name_;
};

}  // namespace tetris_clone
