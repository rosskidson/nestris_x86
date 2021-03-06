#include "nestris_x86.hpp"

#include <chrono>
#include <ctime>
#include <memory>
#include <thread>  //sleep until

#include "assets.hpp"
#include "drawers/olc_drawer.hpp"
#include "frame_processors/game_processor.hpp"
#include "frame_processors/keyboard_config_processor.hpp"
#include "frame_processors/level_screen_processor.hpp"
#include "frame_processors/option_screen_processor.hpp"
#include "input_devices/olc_gamepad.hpp"
#include "input_devices/olc_keyboard.hpp"
#include "key_defines.hpp"
#include "option.hpp"
#include "utils/logging.hpp"

namespace nestris_x86 {

constexpr int NTSC_frame_ns = static_cast<int>((1.0 / NTSC_FREQUENCY) * 1e9);

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

KeyEvents NestrisX86::getKeyEvents() {
  dynamic_cast<OlcGamePad &>(*gamepad_input_).detectAndInit();
  KeyEvents ret_val{};
  for (const auto &[action, keyboard_key] : keyboard_key_bindings_) {
    const auto &gamepad_key = gamepad_key_bindings_.at(action);
    const auto new_keyboard_state = keyboard_input_->getKeyState(keyboard_key);
    const auto new_gamepad_state = gamepad_input_->getKeyState(gamepad_key);
    const auto new_key_state = new_gamepad_state | new_keyboard_state;
    ret_val[action] = getButtonState(key_states_.at(action), new_key_state);
    key_states_[action] = new_key_state;
  }
  return ret_val;
}

NestrisX86::NestrisX86()
    : sample_player_{std::make_shared<sound::SoundPlayer>()},
      sprite_provider_{std::make_shared<SpriteProvider>()},
      keyboard_input_{std::make_unique<OlcKeyboard>(*this)},
      keyboard_key_bindings_{getDefaultKeyBindings(*keyboard_input_)},
      gamepad_input_{std::make_unique<OlcGamePad>()},
      gamepad_key_bindings_{getDefaultGamePadBindings(*gamepad_input_)},
      game_options_{std::make_shared<GameOptions>()},
      game_frame_processor_{std::make_shared<GameProcessor>(
          GameOptions{}, std::make_unique<OlcDrawer>(*this), sample_player_, sprite_provider_)},
      level_menu_processor_{std::make_shared<LevelScreenProcessor>(
          std::make_unique<OlcDrawer>(*this), sample_player_, sprite_provider_)},
      option_menu_processor_{std::make_shared<OptionScreenProcessor>(
          std::make_unique<OlcDrawer>(*this), sample_player_, sprite_provider_)},
      keyboard_config_processor_{std::make_shared<KeyboardConfigProcessor>(
          std::make_unique<OlcDrawer>(*this), sample_player_, std::make_unique<OlcKeyboard>(*this),
          keyboard_key_bindings_)},
      gamepad_config_processor_{std::make_shared<GamePadConfigProcessor>(
          std::make_unique<OlcDrawer>(*this), sample_player_, std::make_unique<OlcGamePad>(),
          gamepad_key_bindings_)},
      active_processor_{level_menu_processor_},
      key_states_{initializeKeyStatesFromBindings(keyboard_key_bindings_)},
      frame_end_{},
      single_frame_{NTSC_frame_ns} {
  sAppName = "NestrisX86";

  if (LOAD_FROM_BINARY) {
    if (not loadSoundAssets(*sample_player_)) {
      throw std::runtime_error("Failed loading sound samples.");
    }
  } else {
    if (not loadSoundAssets("./assets/sounds/", *sample_player_)) {
      throw std::runtime_error("Failed loading sound samples.");
    }
  }
}

bool NestrisX86::OnUserCreate() {
  if (ScreenWidth() != 256 || ScreenHeight() != 225) {
    LOG_ERROR("Screen size must be set to 256x225 for this application.");
    return false;
  }

  this->SetPixelMode(olc::Pixel::MASK);
  frame_end_ = Clock::now() + single_frame_;
  return true;
}

bool NestrisX86::OnUserUpdate(float fElapsedTime) {
  const auto key_events = getKeyEvents();
  const auto signal = active_processor_->processFrame(key_events);
  processProgramFlowSignal(signal);
  sleepUntilNextFrame();
  return not(GetKey(olc::Key::Q).bHeld || signal == ProgramFlowSignal::EndProgram);
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
  options.show_das_bar = getBoolOption(*option_map.at("show_das_meter"));
  options.show_controls = getBoolOption(*option_map.at("show_controls"));
  options.hard_drop = getBoolOption(*option_map.at("hard_drop"));
  options.wall_kick = getBoolOption(*option_map.at("wall_kick"));
  options.statistics_mode =
      statisticsModeFromString(getStringOption(*option_map.at("statistics_mode")));
  options.rng_type = rngTypeFromString(getStringOption(*option_map.at("rng_type")));
  return options;
}

void NestrisX86::processProgramFlowSignal(const ProgramFlowSignal &signal) {
  if (signal == ProgramFlowSignal::StartGame) {
    auto options = menuOptionsToGameOptions(option_menu_processor_->getOptions());
    options.level = level_menu_processor_->getSelectedLevel();
    single_frame_ = Duration_ns{static_cast<int>((1.0 / options.game_frequency) * 1e9)};
    game_frame_processor_->reset(options);
    active_processor_ = game_frame_processor_;
  } else if (signal == ProgramFlowSignal::LevelSelectorScreen) {
    active_processor_ = level_menu_processor_;
  } else if (signal == ProgramFlowSignal::OptionsScreen) {
    if (active_processor_ == keyboard_config_processor_) {
      keyboard_key_bindings_ = keyboard_config_processor_->getKeyBindings();
    } else if (active_processor_ == gamepad_config_processor_) {
      gamepad_key_bindings_ = gamepad_config_processor_->getKeyBindings();
    }
    active_processor_ = option_menu_processor_;
  } else if (signal == ProgramFlowSignal::KeyboardConfigScreen) {
    active_processor_ = keyboard_config_processor_;
  } else if (signal == ProgramFlowSignal::ControllerConfigScreen) {
    active_processor_ = gamepad_config_processor_;
  }
}

void NestrisX86::sleepUntilNextFrame(const bool debug) {
  if (Clock::now() > frame_end_) {
    if (debug) {
      LOG_ERROR(
          "Runtime error: Game code is not finishing in time. The game will not run at the "
          "intended frequency.");
    }
    const auto now_us =
        std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch());
    const auto end_of_frame_us =
        std::chrono::duration_cast<std::chrono::microseconds>(frame_end_.time_since_epoch());
    const auto overrun_us = now_us.count() - end_of_frame_us.count();
    if (debug) {
      LOG_INFO("Now (us):          " << now_us.count());
      LOG_INFO("End of frame (us): " << end_of_frame_us.count());
      LOG_INFO("Time overrun (us): " << overrun_us);
    }

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
 */

/**
 * TODO:
 *
 * Must do:
 * - Full test on linux/mac/windows
 * - Finish readme
 *
 *
 * Like to have:
 * - Change default gamepad bindings for windows (up and down reversed)
 * - cancel button on controller config screen
 * - Save config option
 * - Compress binary data
 *
 *
 * More ambitious:
 * - A tetromino set loader to make the game fully deterministic
 *      (don't forget the rand() in line clear)
 * - Record all inputs, implement a replay functionality
 * - Hold piece
 *
 *
 * Bugs:
 * - timing overrun errors in the menu on macos and windows in the first few frames
 *
 *
 * Refactor/code improvements:
 * - make an asset provider that returns anys for sounds and images
 * - Remove magic numbers in game_logic (entry delay, line clear frame numbers)
 * - Remove std::rand() in line clear
 */

}  // namespace nestris_x86
