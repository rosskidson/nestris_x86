#pragma once

#include <string>

namespace tetris_clone {

class InputInterface {
 public:
  using KeyCode = int;

  virtual ~InputInterface() = default;
  virtual bool getKeyState(const KeyCode key_code) const = 0;

  virtual KeyCode getPressedKey() const = 0;

  virtual std::string keyCodeToStr(const KeyCode key_code) const = 0;
  virtual KeyCode lookupKeyCode(const std::string& key_name) const = 0;
};

}  // namespace tetris_clone
