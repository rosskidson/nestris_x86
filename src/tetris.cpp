#include "tetris.hpp"

#include <chrono>
#include <ctime>
#include <memory>
#include <thread>  //sleep until

#include "assets.hpp"
#include "frame_processors/game_processor.hpp"
#include "frame_processors/keyboard_config_processor.hpp"
#include "frame_processors/level_screen_processor.hpp"
#include "frame_processors/option_screen_processor.hpp"
#include "key_defines.hpp"
#include "olc_drawer.hpp"
#include "option.hpp"
#include "utils/logging.hpp"

namespace tetris_clone {

constexpr int NTSC_frame_ns = (1.0 / NTSC_FREQUENCY) * 1e9;

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

KeyStates initializeKeyStatesFromBindings(const KeyBindings &key_bindings) {
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
    : sample_player_{std::make_shared<sound::SoundPlayer>()},
      sprite_provider_{std::make_shared<SpriteProvider>("./assets/images/")},
      game_options_{std::make_shared<GameOptions>()},
       game_frame_processor_{std::make_shared<GameProcessor>(
          GameOptions{}, std::make_unique<OlcDrawer>(*this), sample_player_, sprite_provider_)},
      level_menu_processor_{std::make_shared<LevelScreenProcessor>(
          std::make_unique<OlcDrawer>(*this), sample_player_, sprite_provider_)},
      option_menu_processor_{std::make_shared<OptionScreenProcessor>(
          std::make_unique<OlcDrawer>(*this), sample_player_, sprite_provider_)},
      keyboard_config_processor_{std::make_shared<KeyboardConfigProcessor>(
          std::make_unique<OlcDrawer>(*this), sample_player_)},
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
    LOG_ERROR("Screen size must be set to 256x225 for this application.");
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
  for (const auto &[action, key] : key_bindings_) {
    const auto new_key_state = GetKey(key).bHeld;
    ret_val[action] = getButtonState(last_key_states.at(action), new_key_state);
    last_key_states[action] = new_key_state;
  }
  return ret_val;
}

TetrisType getGravityOption(const OptionInterface &option) {
  return option.getSelectedOptionText() == "NTSC" ? TetrisType::NTSC : TetrisType::PAL;
}

void dasDelaysToCharges(const int das_initial_delay, const int das_repeat_delay,
                        int &das_full_charge, int &das_min_charge) {
  if (das_repeat_delay > das_initial_delay) {
    das_full_charge = das_repeat_delay;
    das_min_charge = 0;
  } else {
    das_full_charge = das_initial_delay;
    das_min_charge = das_initial_delay - das_repeat_delay;
  }
}

GameOptions menuOptionsToGameOptions(const OptionScreenProcessor::OptionMap &option_map) {
  GameOptions options{};
  options.game_frequency = getIntOption(*option_map.at("refresh_frequency"));
  const int das_repeat_delay = getIntOption(*option_map.at("das_repeat_delay_frames"));
  const int das_initial_delay = getIntOption(*option_map.at("das_initial_delay_frames"));
  dasDelaysToCharges(das_initial_delay, das_repeat_delay, options.das_full_charge,
                     options.das_min_charge);
  options.gravity_type = getGravityOption(*option_map.at("gravity_mode"));
  options.show_entry_delay = getBoolOption(*option_map.at("show_entry_delay"));
  options.show_das_bar = getBoolOption(*option_map.at("show_das_meter"));
  options.show_controls = getBoolOption(*option_map.at("show_controls"));
  return options;
}

void TetrisClone::processProgramFlowSignal(const ProgramFlowSignal &signal) {
  if (signal == ProgramFlowSignal::StartGame) {
    auto options = menuOptionsToGameOptions(option_menu_processor_->getOptions());
    options.level = level_menu_processor_->getSelectedLevel();
    single_frame_ = Duration_ns{static_cast<int>((1.0 / options.game_frequency) * 1e9)};
    game_frame_processor_->reset(options);
    active_processor_ = game_frame_processor_;
  } else if (signal == ProgramFlowSignal::LevelSelectorScreen) {
    active_processor_ = level_menu_processor_;
  } else if (signal == ProgramFlowSignal::OptionsScreen) {
    active_processor_ = option_menu_processor_;
  } else if (signal == ProgramFlowSignal::KeyboardConfigScreen) {
    active_processor_ = keyboard_config_processor_;
  }
  // das_profile",
  // hard_drop",
  // wall_kick",
  // show_das_chain",
  // show_wall_charges",
  // staistics mode",
}

void TetrisClone::sleepUntilNextFrame() {
  if (Clock::now() > frame_end_) {
    LOG_ERROR(
        "Runtime error: Game code is not finishing in time. The game will not run at the intended "
        "frequency.");
    const auto now_us =
        std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch());
    const auto end_of_frame_us =
        std::chrono::duration_cast<std::chrono::microseconds>(frame_end_.time_since_epoch());
    const auto overrun_us = now_us.count() - end_of_frame_us.count();
    LOG_INFO("Now (us):          " << now_us.count());
    LOG_INFO("End of frame (us): " << end_of_frame_us.count());
    LOG_INFO("Time overrun (us): " << overrun_us);

    // If the overrun is more than a frame, reset so the game doesn't have to play catch up.
    if (overrun_us > single_frame_.count() / 1000) {
      frame_end_ = Clock::now();
      const auto reset_us =
          std::chrono::duration_cast<std::chrono::microseconds>(frame_end_.time_since_epoch());
      LOG_INFO("Frame reset to : " << reset_us.count());
    }
  }

  std::this_thread::sleep_until(frame_end_);
  frame_end_ += single_frame_;
}

/**
 * TODO:
 *
 * - timing overrun errors in the menu on macos
 * - configure keyboard
 * - configure controller
 * - Restructure layout for
 *     -ARE box
 *     -7 digit score
 *     -tetromino hold
 *     -wall charge signal
 *     -DAS chain counter
 * - Asset loading from binary
 * - Press down scoring
 * - Hard drop
 * - Statistics mode
 * - Hold
 * - Remove magic numbers in game_logic (entry delay, line clear frame numbers)
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
