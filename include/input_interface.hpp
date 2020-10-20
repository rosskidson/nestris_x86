#pragma once

#include <string>

namespace tetris_clone {

class InputInterface {
 public:
  virtual ~InputInterface() = default;
  virtual bool getKeyState(const int key_code) const = 0;

  virtual int getPressedKey() const = 0;

  virtual std::string keyCodeToStr(const int key_code) const = 0;
  virtual int lookupKeyCode(const std::string& key_name) const = 0;
};

}  // namespace tetris_clone
