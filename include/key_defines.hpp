#pragma once

#include <map>

#include "olcPixelGameEngine.h"

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

const std::array<std::string, static_cast<int>(KeyAction::COUNT)> action_names{
    "Up",
    "Down",
    "Left",
    "Right",
    "Rotate Clockwise",
    "Rotate AntiClockwise,"
    "Start"};


inline std::string keyActionToString(const KeyAction& key_action) {
  return action_names[static_cast<int>(key_action)];
}

constexpr int key_action_size = static_cast<int>(KeyAction::COUNT);

using KeyBindings = std::map<KeyAction, olc::Key>;
using KeyStates = std::map<KeyAction, bool>;
using KeyEvents = std::map<KeyAction, KeyEvent>;

}  // namespace tetris_clone
