#pragma once

#include <chrono>
#include <memory>

#include "assets.hpp"
#include "frame_processors/frame_processor_interface.hpp"
#include "frame_processors/game_processor.hpp"
#include "frame_processors/gamepad_config_processor.hpp"
#include "frame_processors/keyboard_config_processor.hpp"
#include "frame_processors/level_screen_processor.hpp"
#include "frame_processors/option_screen_processor.hpp"
#include "game_states.hpp"
#include "input_devices/input_interface.hpp"
#include "key_defines.hpp"
#include "olcPixelGameEngine.h"
#include "sound.hpp"
#include "utils/logging.hpp"

namespace nestris_x86 {

using Clock = std::chrono::high_resolution_clock;
using Duration_ns = std::chrono::duration<int, std::nano>;

class TetrisClone : public olc::PixelGameEngine {
 public:
  TetrisClone();

  bool OnUserCreate() override;

  bool OnUserUpdate(float fElapsedTime) override;

 private:
  void processProgramFlowSignal(const ProgramFlowSignal& signal);

  void sleepUntilNextFrame();

  KeyEvents getKeyEvents();

  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  std::unique_ptr<InputInterface> keyboard_input_;
  std::unique_ptr<InputInterface> gamepad_input_;
  KeyBindings keyboard_key_bindings_;
  KeyBindings gamepad_key_bindings_;
  std::shared_ptr<GameOptions> game_options_;
  std::shared_ptr<GameProcessor> game_frame_processor_;
  std::shared_ptr<LevelScreenProcessor> level_menu_processor_;
  std::shared_ptr<OptionScreenProcessor> option_menu_processor_;
  std::shared_ptr<KeyboardConfigProcessor> keyboard_config_processor_;
  std::shared_ptr<GamePadConfigProcessor> gamepad_config_processor_;
  std::shared_ptr<FrameProcessorInterface> active_processor_;
  KeyStates key_states_;
  std::chrono::time_point<std::chrono::high_resolution_clock> frame_end_;
  Duration_ns single_frame_;
};

}  // namespace nestris_x86
