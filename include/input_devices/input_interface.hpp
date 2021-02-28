#pragma once

#include <string>
#include <vector>

namespace nestris_x86 {

class InputInterface {
 public:
  using KeyCode = int;

  virtual ~InputInterface() = default;
  virtual bool getKeyState(const KeyCode key_code) = 0;

  virtual KeyCode getPressedKey() = 0;

  virtual std::string keyCodeToStr(const KeyCode key_code) const = 0;
  virtual KeyCode lookupKeyCode(const std::string& key_name) const = 0;

  virtual KeyCode getNullKey() const = 0;

  // TODO:: Make a gamepad interface that inherits from Input Interface for this.
  //   That way keyboard etc. doesn't need a no-op implementation.
  virtual void registerAxisAsButton(const int axis_number, const double axis_at_rest,
                                    const double axis_pressed) = 0;

  struct RegisteredAxisMovement {
    int axis_number;
    double axis_at_rest;
    double axis_pressed;
  };
  virtual std::vector<RegisteredAxisMovement> getRegisteredAxes() const = 0;
};

}  // namespace nestris_x86
