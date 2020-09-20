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
#include "game_logic.hpp"
#include "logging.hpp"
#include "sound.hpp"

namespace tetris_clone {

constexpr int GRAVITY_FIRST_FRAME = 100;

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

Tetromino TetrisClone::getRandomTetromino() {
  return Tetromino(random_generator_(real_rng_));
}

bool TetrisClone::spawnNewTetromino(GameState<> &state) {
  state.active_tetromino = {state.next_tetromino, 5, 0, 0};
  state_.tetromino_counts[static_cast<int>(state_.active_tetromino.tetromino)]++;
  state.next_tetromino = getRandomTetromino();
  state.spawn_new_tetromino = false;
  return not tetrominoCollision(state.grid, state.active_tetromino);
}

GameState<> TetrisClone::getNewState(const int level) {
  auto state = GameState<>{};
  state.level = level;
  state.active_tetromino = {getRandomTetromino(), 5, 0, 0};
  state.next_tetromino = getRandomTetromino();
  state.tetromino_counts[static_cast<int>(state.active_tetromino.tetromino)]++;
  state.gravity_counter = GRAVITY_FIRST_FRAME;
  state.lines_until_next_level = linesToClearFromStartingLevel(state.level);
  return state;
}

bool TetrisClone::runGameSingleFrame() {
  if (state_.topped_out) {
    const auto key_events = getKeyEvents(key_states_);
    const bool end_game = updateTopOutState(key_events, top_out_frame_counter_, state_);
    if (end_game) {
      return false;
    }
    renderer_.renderGameState(state_, debug_mode_, key_states_);
  } else if (state_.paused) {
    renderer_.renderPaused();
    const auto key_events = getKeyEvents(key_states_);
    if (key_events.at(KeyAction::Start).pressed) {
      state_.paused = false;
    }
  } else if (not entryDelay(state_)) {
    if (state_.spawn_new_tetromino) {
      const bool topped_out = not spawnNewTetromino(state_);
      if (topped_out) {
        // Lock the active tetromino and move it off the grid. This is to stop the active
        // tetromino interfering with the 'curtain' animation.
        state_.grid = addTetrominoToGrid(state_.grid, state_.active_tetromino);
        state_.active_tetromino.y = -10;
        state_.topped_out = true;
        sample_player_.playSample("top_out");
      }
    }

    const auto key_events = getKeyEvents(key_states_);
    processKeyEvents(key_events, sample_player_, state_);

    const bool tetromino_locked = applyGravity(state_);
    if (tetromino_locked) {
      sample_player_.playSample("lock");
      auto lines_cleared = checkForLineClears(state_);
      if (not lines_cleared.empty()) {
        updateEntryDelayForLineClear(state_.entry_delay_counter);
        line_clear_info_ = LineClearAnimationInfo{lines_cleared, state_.entry_delay_counter};
      }
    }
    renderer_.renderGameState(state_, debug_mode_, key_states_);
  } else {
    animateLineClear(sample_player_, state_, line_clear_info_);
    if (line_clear_info_.rows.size() == 4) {
      renderer_.doTetrisFlash(line_clear_info_.animation_frame);
    }
    // When the animation is almost over, update the score.
    if (line_clear_info_.animation_frame == 4) {
      updateScoreAndLevel(line_clear_info_.rows.size(), sample_player_, state_);
    }
    getKeyEvents(key_states_);
    renderer_.renderGameState(state_, debug_mode_, key_states_);
    --state_.entry_delay_counter;
  }
  return true;
}

bool TetrisClone::runMenuSingleFrame() {
  menu_ = false;

  return true;
}

void TetrisClone::sleepUntilNextFrame() {
  std::this_thread::sleep_until(frame_end_);
  frame_end_ += single_frame;
}

// TODO:: Add asset loading to constructor/OnUserCreate and add error
// handling.
TetrisClone::TetrisClone(const int start_level)
    : renderer_{*this, "./assets/images"},
      sample_player_{},
      state_{},
      menu_{true},
      real_rng_{},
      random_generator_(0, 6),
      key_states_{},
      key_bindings_{getKeyBindings()},
      debug_mode_{true},
      frame_end_{},
      line_clear_info_{},
      top_out_frame_counter_{} {
  sAppName = "TetrisClone";
  // Initialize key states from key bindings.
  for (const auto &pair : key_bindings_) {
    key_states_[pair.first] = false;
  }
  state_ = getNewState(start_level);
}

bool TetrisClone::OnUserCreate() {
  if (ScreenWidth() != 256 || ScreenHeight() != 225) {
    LOG_ERROR("Screen size must be set to 256x240 for this application.");
    return false;
  }

  if (not loadSoundAssets("./assets/sounds/", sample_player_)) {
    return false;
  }
  renderer_.init();

  frame_end_ = Clock::now() + single_frame;

  return true;
}

bool TetrisClone::OnUserUpdate(float fElapsedTime) {
  bool frame_success{};
  if(menu_) {
    frame_success = runMenuSingleFrame();
  } else {
    frame_success = runGameSingleFrame();
  }

  sleepUntilNextFrame();
  frame_success = frame_success && not GetKey(olc::Key::Q).bHeld;
  return frame_success;
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
 * - Move rendering functions behind an interface
 *      - Render class hold an interface pointer
 */

}  // namespace tetris_clone
