#include "olc_gamepad.hpp"

#include <string>

#include "utils/logging.hpp"
#include "Contributions/olcPGEX_Gamepad.h"

namespace tetris_clone {

namespace {
// clang-format off
// copy pasted from olc source code and formatted to strings.
const std::map<int, std::string> CODE_TO_NAME{
  {-1,  "NONE"},
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
  {16, "DPAD_D"},
  {17, "DPAD_U"}
};
// clang-format on
constexpr int OLC_NUMBER_OF_KEYS = 18;

std::map<std::string, int> getReverseLookup(const std::map<int, std::string>& code_to_name) {
  std::map<std::string, int> name_to_code{};
  for (const auto& [id, name] : code_to_name) {
    name_to_code[name] = id;
  }
  return name_to_code;
}

const std::map<std::string, int> NAME_TO_CODE = getReverseLookup(CODE_TO_NAME);

InputInterface::KeyCode olcToKeyCode(const olc::GPButtons& key) {
  return static_cast<InputInterface::KeyCode>(key);
}

olc::GPButtons keyCodeToOlc(const InputInterface::KeyCode key_code) {
  return static_cast<olc::GPButtons>(key_code);
}
}  // namespace

class OlcGamePad::Impl {
 public:
  Impl() : gamepads_{}, gamepad_ptr_{} {
    olc::GamePad::init();
    gamepads_ = olc::GamePad::getGamepads();
  }

  bool detectAndInit() {
    if (gamepad_ptr_->valid) {
      return true;
    }
    for (auto& gamepad : gamepads_) {
      gamepad.poll();
      for (int i = 0; i < OLC_NUMBER_OF_KEYS; ++i) {
        if (gamepad.getButton(static_cast<olc::GPButtons>(i)).bHeld) {  // Use keyCodeToOlc
          gamepad_ptr_ = &gamepad;
        }
      }
    }
    return false;
  }

  bool getKeyState(const KeyCode key_code) const {
    if (not gamepad_ptr_->valid) {
      return false;
    }
    gamepad_ptr_->poll();
    return gamepad_ptr_->getButton(keyCodeToOlc(key_code)).bHeld;
  }

  InputInterface::KeyCode getPressedKey() const {
    for (int i = 0; i < 18; ++i) {
      if (getKeyState(i)) {
        return i;
      }
    }
    return getNullKey();
  }

  std::string keyCodeToStr(const KeyCode key_code) const {
    if (not CODE_TO_NAME.count(key_code)) {
      LOG_ERROR("Unknown key code `" << key_code << "`.");
      return std::to_string(key_code);
    }
    return CODE_TO_NAME.at(key_code);
  }

  InputInterface::KeyCode lookupKeyCode(const std::string& key_name) const {
    if (not NAME_TO_CODE.count(key_name)) {
      LOG_ERROR("No key code found for name `" << key_name << "`.");
      return getNullKey();
    }
    return NAME_TO_CODE.at(key_name);
  }

  InputInterface::KeyCode getNullKey() const { return -1; }

 private:
  std::vector<olc::GamePad> gamepads_;
  olc::GamePad* gamepad_ptr_;
};

OlcGamePad::OlcGamePad() : pimpl_{std::make_unique<OlcGamePad::Impl>()} {}

bool OlcGamePad::detectAndInit() {
  return pimpl_->detectAndInit();
}

bool OlcGamePad::getKeyState(const KeyCode key_code) const {
  return pimpl_->getKeyState(key_code);
}

InputInterface::KeyCode OlcGamePad::getPressedKey() const {
  return pimpl_->getPressedKey();
}

std::string OlcGamePad::keyCodeToStr(const KeyCode key_code) const {
  return pimpl_->keyCodeToStr(key_code);
}

InputInterface::KeyCode OlcGamePad::lookupKeyCode(const std::string& key_name) const {
  return pimpl_->lookupKeyCode(key_name);
}

InputInterface::KeyCode OlcGamePad::getNullKey() const {
  return pimpl_->getNullKey();
}

}  // namespace tetris_clone
