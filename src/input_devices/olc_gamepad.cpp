#include "input_devices/olc_gamepad.hpp"

#include <iso646.h>
#include <string>

#include "utils/logging.hpp"

#if defined(__unix__) || defined(_WIN32) || defined(WIN32) 
#include "olcPGEX_Gamepad.h"
#endif

namespace nestris_x86 {

#if __APPLE__
// Define dummy impl.
class OlcGamePad::Impl {
 public:
  bool detectAndInit() { return false; }
  bool getKeyState(const KeyCode key_code) { return false; }
  InputInterface::KeyCode getPressedKey() { return getNullKey(); }
  std::string keyCodeToStr(const KeyCode key_code) const { return "Invalid key"; }
  InputInterface::KeyCode lookupKeyCode(const std::string& key_name) const { return getNullKey(); }
  InputInterface::KeyCode getNullKey() const { return -1; }
};
#else
namespace {
// clang-format off
// copy pasted from olc source code and formatted to strings.
// These are not entirely accurate. See the getDefaultKeyBindings function for default bindings 
// across different operating systems.
const std::map<int, std::string> CODE_TO_NAME{
  {-1,  "NONE"},
  {0,  "FACE_L"},
  {1,  "FACE_D"},
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
    if (gamepad_ptr_ && gamepad_ptr_->valid) {
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

  bool getKeyState(const KeyCode key_code) {
    if (not gamepad_ptr_ || not gamepad_ptr_->valid) {
      return false;
    }
    gamepad_ptr_->poll();
    return gamepad_ptr_->getButton(keyCodeToOlc(key_code)).bHeld;
  }

  InputInterface::KeyCode getPressedKey() {
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

#endif  // #if else __APPLE__

OlcGamePad::OlcGamePad() : pimpl_{std::make_unique<OlcGamePad::Impl>()} {}

OlcGamePad::~OlcGamePad() = default;

bool OlcGamePad::detectAndInit() {
  return pimpl_->detectAndInit();
}

bool OlcGamePad::getKeyState(const KeyCode key_code) {
  return pimpl_->getKeyState(key_code);
}

InputInterface::KeyCode OlcGamePad::getPressedKey() {
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

}  // namespace nestris_x86
