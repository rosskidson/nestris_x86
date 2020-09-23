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

constexpr int NTSC_frame_ns = (1.0 / 60.0) * 1e9;

KeyBindings getKeyBindings() {
  KeyBindings key_bindings;
  key_bindings[KeyAction::Up] = olc::Key::UP;
  key_bindings[KeyAction::Down] = olc::Key::DOWN;
  key_bindings[KeyAction::Left] = olc::Key::LEFT;
  key_bindings[KeyAction::Right] = olc::Key::RIGHT;
  key_bindings[KeyAction::RotateClockwise] = olc::Key::X;      // NES gamepad A
  key_bindings[KeyAction::RotateAntiClockwise] = olc::Key::Z;  // NES gamepad B
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
          std::make_shared<LevelScreenProcessor>(renderer_, sample_player_)},
      active_processor_{level_menu_processor_},
      key_bindings_{getKeyBindings()},
      key_states_{initializeKeyStatesFromBindings(key_bindings_)},
      frame_end_{},
      single_frame_{NTSC_frame_ns} {
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
  frame_end_ = Clock::now() + single_frame_;

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
    GameOptions options{};
    //options.game_frequency = 50;
    single_frame_ = Duration_ns{static_cast<int>((1.0 / options.game_frequency) * 1e9)};
    options.level = level_menu_processor_->getSelectedLevel();
    //options.das_full_charge = 12;
    //options.das_min_charge = 8;
    //options.gravity_type = TetrisType::PAL;
    game_frame_processor_ =
        std::make_shared<GameProcessor>(options, renderer_, sample_player_);
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
  frame_end_ += single_frame_;
}

/**
 * TODO:
 *
 * - Offer PAL level speeds
 * - Options screen
 * - High score functionality
 * - Asset loading from binary
 * - Press down scoring
 * - Remove magic numbers in game_logic (entry delay, line clear frame numbers)
 * - BUG: pause doesn't work during entry delay
 * - BUG: pausing for a long time messes up sleep until frame end
 * - Press down hold between blocks?
 * - precise timing checks:
 *    - line clear sfx
 *    - delay between death and death sound
 *    - delay between death and end animation
 *    - end animation speed
 * - Remove full path from logging
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
