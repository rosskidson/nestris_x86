#pragma once

#include <map>
#include <array>

#include "input_interface.hpp"

namespace tetris_clone {

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
    "Up",
    "Down",
    "Left",
    "Right",
    "Rotate Clockwise",
    "Rotate C-Clockwise",
    "Start"};


inline std::string keyActionToString(const KeyAction& key_action) {
  return action_names[static_cast<int>(key_action)];
}


using KeyBindings = std::map<KeyAction, InputInterface::KeyCode>;
using KeyStates = std::map<KeyAction, bool>;
using KeyEvents = std::map<KeyAction, KeyEvent>;

}  // namespace tetris_clone
