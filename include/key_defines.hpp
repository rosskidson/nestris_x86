#pragma once

#include <map>

#include "olcPixelGameEngine.h"

namespace tetris_clone {

struct KeyEvent {
  bool pressed{};
  bool released{};
  bool held{};
};

enum class KeyAction { Up, Down, Left, Right, RotateLeft, RotateRight, Start, Quit };

using KeyBindings = std::map<KeyAction, olc::Key>;
using KeyStates = std::map<KeyAction, bool>;
using KeyEvents = std::map<KeyAction, KeyEvent>;

} // namespace tetris_clone
