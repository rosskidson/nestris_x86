#include <string>

#include "olc_gamepad.hpp"

namespace tetris_clone {

const std::map<std::string, int> name_to_code_{};
const std::map<int, std::string> code_to_name_{};

class OlcGamePad::Impl {
};

OlcGamePad::OlcGamePad() : pimpl_{} {}

OlcGamePad::~OlcGamePad() = default;

bool OlcGamePad::detectAndInit() {
  return false;
}

bool OlcGamePad::getKeyState(const KeyCode key_code) const {
  return false;
}

InputInterface::KeyCode OlcGamePad::getPressedKey() const {
  return getNullKey();
}

std::string OlcGamePad::keyCodeToStr(const KeyCode key_code) const {
  return std::to_string(key_code);
}

InputInterface::KeyCode OlcGamePad::lookupKeyCode(const std::string& key_name) const {
  return getNullKey();
}

InputInterface::KeyCode OlcGamePad::getNullKey() const {
  return -1;
}

}  // namespace tetris_clone
