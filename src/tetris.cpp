#include "tetris.hpp"

#include <chrono>
#include <iomanip>
#include <memory>
#include <thread>  //sleep until

#include "assets.hpp"
#include "game_processor.hpp"
#include "game_states.hpp"
#include "key_defines.hpp"
#include "level_screen_processor.hpp"
#include "logging.hpp"

namespace tetris_clone {

using Clock = std::chrono::high_resolution_clock;
using Duration_us = std::chrono::duration<int, std::nano>;
using Duration_ms = std::chrono::duration<int, std::milli>;
constexpr Duration_us single_frame(16666667);

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

KeyStates initializeKeyStatesFromBindings(const KeyBindings& key_bindings) {
  KeyStates key_states{};
  for (const auto &pair : key_bindings) {
    key_states[pair.first] = false;
  }
  return key_states; 
}

KeyEvent getButtonState(const bool button_old_state, const bool button_new_state) {
  KeyEvent event{};
  event.pressed = not button_old_state && button_new_state;
  event.released = button_old_state && not button_new_state;
  event.held = button_old_state && button_new_state;
  return event;
}

TetrisClone::TetrisClone()
    : renderer_{std::make_shared<Renderer>(*this, "./assets/images")},
      sample_player_{std::make_shared<sound::SoundPlayer>()},
      game_options_{std::make_shared<GameOptions>()},
      game_frame_processor_{
          std::make_shared<GameProcessor>(GameOptions{}, renderer_, sample_player_)},
      level_menu_processor_{
          std::make_shared<LevelScreenProcessor>(renderer_, sample_player_, game_options_)},
      active_processor_{level_menu_processor_},
      key_bindings_{getKeyBindings()},
      key_states_{initializeKeyStatesFromBindings(key_bindings_)},
      frame_end_{} {
  sAppName = "TetrisClone";

  if (not loadSoundAssets("./assets/sounds/", *sample_player_)) {
    throw std::runtime_error("Failed loading sound samples.");
  }
}

bool TetrisClone::OnUserCreate() {
  if (ScreenWidth() != 256 || ScreenHeight() != 225) {
    LOG_ERROR("Screen size must be set to 256x240 for this application.");
    return false;
  }
  this->SetPixelMode(olc::Pixel::MASK);
  frame_end_ = Clock::now() + single_frame;

  return true;
}

bool TetrisClone::OnUserUpdate(float fElapsedTime) {
  const auto key_events = getKeyEvents(key_states_);
  const auto signal = active_processor_->processFrame(key_events);
  processProgramFlowSignal(signal);
  sleepUntilNextFrame();
  return not(GetKey(olc::Key::Q).bHeld || signal == ProgramFlowSignal::EndProgram);
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

void TetrisClone::processProgramFlowSignal(const ProgramFlowSignal &signal) {
  if (signal == ProgramFlowSignal::StartGame) {
    game_frame_processor_ =
        std::make_shared<GameProcessor>(*game_options_, renderer_, sample_player_);
    active_processor_ = game_frame_processor_;
  } else if (signal == ProgramFlowSignal::FrameProcessorEnded) {
    active_processor_ = level_menu_processor_;
  }
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

/**
 * TODO:
 *
 * - Options screen
 * - Change rotate left and rotate right to A and B
 * - Visualize controls as a controller
 * - Visualize DAS a bit nicer, add visualize das param
 * - Asset loading from binary
 * - Press down scoring
 * - Press down hold between blocks?
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
 * - Make a SpriteProvider, refactor Renderer into multiple render classes
 * - Move rendering functions behind an interface
 *      - Render class hold an interface pointer
 */

}  // namespace tetris_clone
