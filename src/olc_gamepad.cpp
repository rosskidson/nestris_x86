#include "olc_gamepad.hpp"

#include "utils/logging.hpp"

namespace tetris_clone {

// copy pasted from olc source code and formatted to strings.
// clang-format off
const std::map<int, std::string> OlcGamePad::code_to_name_{
  {1,  "FACE_D"},
  {0,  "FACE_L"},
  {2,  "FACE_R"},
  {3,  "FACE_U"},
  {4,  "L1"},
  {6,  "L2"},
  {10, "L3"},
  {5,  "R1"},
  {7,  "R2"},
  {11, "R3"},
  {8,  "SELECT"},
  {9,  "START"},
  {14, "DPAD_L"},
  {15, "DPAD_R"},
  {16, "DPAD_U"},
  {17, "DPAD_D"}
};
// clang-format on

std::map<std::string, int> getLookupTable(const std::map<int, std::string>& code_to_name) {
  std::map<std::string, int> name_to_code{};
  for (const auto& [id, name] : code_to_name) {
    name_to_code[name] = id;
  }
  return name_to_code;
}

const std::map<std::string, int> OlcGamePad::name_to_code_ =
    getLookupTable(OlcGamePad::code_to_name_);

OlcGamePad::OlcGamePad() : gamepads_{}, gamepad_{}, gamepad_ptr_{&gamepad_} {
  olc::GamePad::init();
  gamepads_ = olc::GamePad::getGamepads();
}

bool OlcGamePad::detectAndInit() {
  if (gamepad_.valid) {
    return true;
  }
  for (auto& gamepad : gamepads_) {
    gamepad.poll();
    for (int i = 0; i < 18; ++i) {
      if (i == 12 || i == 13) {
        continue;
      }
      if (gamepad.getButton(static_cast<olc::GPButtons>(i)).bHeld) {
        gamepad_ = gamepad;
        gamepad_ptr_ = &gamepad_;
      }
    }
  }
  return false;
}

bool OlcGamePad::getKeyState(const KeyCode key_code) const {
  if(not gamepad_.valid) {
    return false;
  }
  gamepad_ptr_->poll();
  return gamepad_ptr_->getButton(keyCodeToOlc(key_code)).bHeld;
}

InputInterface::KeyCode OlcGamePad::getPressedKey() const {
  for (int i = 0; i < 18; ++i) {
    if (getKeyState(i)) {
      return i;
    }
  }
  return getNullKey();
}

std::string OlcGamePad::keyCodeToStr(const KeyCode key_code) const {
  if (not code_to_name_.count(key_code)) {
    LOG_ERROR("Invalid key code `" << key_code << "`.");
    return "INVALID";
  }
  return code_to_name_.at(key_code);
}

InputInterface::KeyCode OlcGamePad::lookupKeyCode(const std::string& key_name) const {
  if (not name_to_code_.count(key_name)) {
    LOG_ERROR("No key code found for name `" << key_name << "`.");
    return getNullKey();
  }
  return name_to_code_.at(key_name);
}

InputInterface::KeyCode OlcGamePad::olcToKeyCode(const olc::GPButtons& key) {
  return static_cast<KeyCode>(key);
}

olc::GPButtons OlcGamePad::keyCodeToOlc(const KeyCode key_code) {
  return static_cast<olc::GPButtons>(key_code);
}

InputInterface::KeyCode OlcGamePad::getNullKey() const {
  return -1;
}

}  // namespace tetris_clone
