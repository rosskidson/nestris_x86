#pragma once

#include <array>
#include <map>

#include "input_devices/input_interface.hpp"

namespace nestris_x86 {

struct KeyEvent {
  bool pressed{};
  bool released{};
  bool held{};
};

enum class KeyAction {
  Up,
  Down,
  Left,
  Right,
  RotateClockwise,      // NES gamepad A.
  RotateAntiClockwise,  // NES gamepad B.
  Start,
  COUNT
};

constexpr int key_action_size = static_cast<int>(KeyAction::COUNT);
const std::array<std::string, key_action_size> action_names{
    "Up", "Down", "Left", "Right", "Rotate CW  (A)", "Rotate CCW (B)", "Start"};

inline std::string keyActionToString(const KeyAction &key_action) {
  return action_names[static_cast<int>(key_action)];
}

using KeyBindings = std::map<KeyAction, InputInterface::KeyCode>;
using KeyStates = std::map<KeyAction, bool>;
using KeyEvents = std::map<KeyAction, KeyEvent>;

inline KeyBindings getDefaultKeyBindings(const InputInterface &key_input) {
  KeyBindings key_bindings;
  key_bindings[KeyAction::Up] = key_input.lookupKeyCode("UP");
  key_bindings[KeyAction::Down] = key_input.lookupKeyCode("DOWN");
  key_bindings[KeyAction::Left] = key_input.lookupKeyCode("LEFT");
  key_bindings[KeyAction::Right] = key_input.lookupKeyCode("RIGHT");
  key_bindings[KeyAction::RotateClockwise] = key_input.lookupKeyCode("X");      // NES gamepad A
  key_bindings[KeyAction::RotateAntiClockwise] = key_input.lookupKeyCode("Z");  // NES gamepad B
  key_bindings[KeyAction::Start] = key_input.lookupKeyCode("ENTER");
  return key_bindings;
}

inline KeyBindings getDefaultGamePadBindings(const InputInterface &key_input) {
  KeyBindings key_bindings;
  key_bindings[KeyAction::Up] = key_input.lookupKeyCode("DPAD_U");
  key_bindings[KeyAction::Down] = key_input.lookupKeyCode("DPAD_D");
  key_bindings[KeyAction::Left] = key_input.lookupKeyCode("DPAD_L");
  key_bindings[KeyAction::Right] = key_input.lookupKeyCode("DPAD_R");
  key_bindings[KeyAction::RotateClockwise] = key_input.lookupKeyCode("FACE_R");  // NES gamepad A
  key_bindings[KeyAction::RotateAntiClockwise] =
      key_input.lookupKeyCode("FACE_D");  // NES gamepad B
  key_bindings[KeyAction::Start] = key_input.lookupKeyCode("START");
  return key_bindings;
}
//#endif

}  // namespace nestris_x86
