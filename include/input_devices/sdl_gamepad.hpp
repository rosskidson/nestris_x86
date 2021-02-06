#pragma once

#include <memory>
#include <string>

#include "input_interface.hpp"

namespace nestris_x86 {

class SdlGamePad : public InputInterface {
 public:
  SdlGamePad();
  ~SdlGamePad();

  bool getKeyState(const KeyCode key_code) override;
  KeyCode getPressedKey() override;

  std::string keyCodeToStr(const KeyCode key_code) const override;
  KeyCode lookupKeyCode(const std::string& key_name) const override;
  KeyCode getNullKey() const override;

 private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;
};

}  // namespace nestris_x86
