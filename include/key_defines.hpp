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

// copy pasted from olc source code and formatted to strings.
// clang-format off
const std::array<std::string, static_cast<int>(85)> key_names{
		"NONE",
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		"K0", "K1", "K2", "K3", "K4", "K5", "K6", "K7", "K8", "K9",
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
		"UP", "DOWN", "LEFT", "RIGHT",
		"SPACE"," TAB"," SHIFT"," CTRL"," INS"," DEL"," HOME"," END"," PGUP"," PGDN",
		"BACK"," ESCAPE"," RETURN"," ENTER"," PAUSE"," SCROLL",
		"NP0"," NP1"," NP2"," NP3"," NP4"," NP5"," NP6"," NP7"," NP8"," NP9",
		"NP_MUL"," NP_DIV"," NP_ADD"," NP_SUB"," NP_DECIMAL"," PERIOD"
};
// clang-format on

inline std::string keyActionToString(const KeyAction& key_action) {
  return action_names[static_cast<int>(key_action)];
}
inline std::string keyToString(const olc::Key& key) {
  return key_names[static_cast<int>(key)];
}

constexpr int key_action_size = static_cast<int>(KeyAction::COUNT);

using KeyBindings = std::map<KeyAction, olc::Key>;
using KeyStates = std::map<KeyAction, bool>;
using KeyEvents = std::map<KeyAction, KeyEvent>;

}  // namespace tetris_clone
