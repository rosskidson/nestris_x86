#include "tetris.hpp"

#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>  //sleep until
#include <tuple>

#include "assets.hpp"
#include "frame_processor_interface.hpp"
#include "game_logic.hpp"
#include "game_processor.hpp"
#include "game_states.hpp"
#include "logging.hpp"
#include "sound.hpp"

namespace tetris_clone {

using Clock = std::chrono::high_resolution_clock;
using Duration_us = std::chrono::duration<int, std::nano>;
using Duration_ms = std::chrono::duration<int, std::milli>;
constexpr Duration_us single_frame(16666667);
// constexpr Duration_ms single_frame(500);

KeyBindings getKeyBindings() {
  KeyBindings key_bindings;
  key_bindings[KeyAction::Up] = olc::Key::UP;
  key_bindings[KeyAction::Down] = olc::Key::DOWN;
  key_bindings[KeyAction::Left] = olc::Key::LEFT;
  key_bindings[KeyAction::Right] = olc::Key::RIGHT;
  key_bindings[KeyAction::RotateLeft] = olc::Key::X;
  key_bindings[KeyAction::RotateRight] = olc::Key::Z;
  key_bindings[KeyAction::Start] = olc::Key::ENTER;
  return key_bindings;
}

KeyEvent getButtonState(const bool button_old_state, const bool button_new_state) {
  KeyEvent event{};
  event.pressed = not button_old_state && button_new_state;
  event.released = button_old_state && not button_new_state;
  event.held = button_old_state && button_new_state;
  return event;
}

KeyEvents TetrisClone::getKeyEvents(KeyStates &last_key_states) {
  KeyEvents ret_val{};
  for (const auto &pair : key_bindings_) {
    const auto &key = pair.first;
    auto new_key_state = GetKey(pair.second).bHeld;
    ret_val[key] = getButtonState(last_key_states.at(key), new_key_state);
    last_key_states[key] = new_key_state;
  }
  return ret_val;
}

ProgramFlowSignal TetrisClone::runMenuSingleFrame(const KeyEvents &key_events) {
  renderer_->renderMenu(menu_state_);
  if (key_events.at(KeyAction::Start).pressed) {
    return ProgramFlowSignal::StartGame;
  }

  return ProgramFlowSignal::FrameSuccess;
}

void TetrisClone::sleepUntilNextFrame() {
  if (Clock::now() > frame_end_) {
    LOG_ERROR(
        "Runtime error: Game code is not finishing in time. The game will not run at the intended "
        "frequency.");
  }
  std::this_thread::sleep_until(frame_end_);
  frame_end_ += single_frame;
}

// TODO:: Add asset loading to constructor/OnUserCreate and add error
// handling.
TetrisClone::TetrisClone(const int start_level)
    : renderer_{std::make_shared<Renderer>(*this, "./assets/images")},
      game_frame_processor_{std::make_unique<GameProcessor>(
          GameOptions{start_level, {}, {}, {}, {true}, {}}, renderer_)},
      menu_state_{},
      menu_{true},
      key_states_{},
      key_bindings_{getKeyBindings()},
      frame_end_{} {
  sAppName = "TetrisClone";
  // Initialize key states from key bindings.
  for (const auto &pair : key_bindings_) {
    key_states_[pair.first] = false;
  }
}

bool TetrisClone::OnUserCreate() {
  if (ScreenWidth() != 256 || ScreenHeight() != 225) {
    LOG_ERROR("Screen size must be set to 256x240 for this application.");
    return false;
  }
  frame_end_ = Clock::now() + single_frame;

  return true;
}

bool TetrisClone::OnUserUpdate(float fElapsedTime) {
  const auto key_events = getKeyEvents(key_states_);

  ProgramFlowSignal return_code;
  if (menu_) {
    return_code = runMenuSingleFrame(key_events);
  } else {
    return_code = game_frame_processor_->processFrame(key_events);
  }

  if (return_code == ProgramFlowSignal::StartGame) {
    menu_ = false;
  }

  sleepUntilNextFrame();
  return not GetKey(olc::Key::Q).bHeld;
}

/**
 * TODO:
 *
 * - Some kind of menu system
 * - Asset loading from binary
 * - Press down scoring
 * - Check rotation consistency/tetromino entry state is correct
 * - precise timing checks:
 *    - line clear sfx
 *    - delay between death and death sound
 *    - delay between death and end animation
 *    - end animation speed
 *
 * IDEAS::
 * - Wall charge animation/signal
 * - Das chain counter/animation
 * - Interface for random generator: allow a tetromino set loader
 *      (don't forget the rand() in line clear)
 * - Record all inputs, implement a replay functionality
 *
 * REFACTOR:
 * - Split main loop into functions
 * - Move rendering functions behind an interface
 *      - Render class hold an interface pointer
 */

}  // namespace tetris_clone
