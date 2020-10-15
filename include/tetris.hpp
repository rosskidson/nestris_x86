#pragma once

#include <chrono>
#include <memory>

#include "assets.hpp"
#include "frame_processor_interface.hpp"
#include "frame_processors/game_processor.hpp"
#include "frame_processors/keyboard_config_processor.hpp"
#include "frame_processors/level_screen_processor.hpp"
#include "frame_processors/option_screen_processor.hpp"
#include "game_states.hpp"
#include "key_defines.hpp"
#include "utils/logging.hpp"
#include "olcPixelGameEngine.h"
#include "rendering.hpp"
#include "sound.hpp"

namespace tetris_clone {

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

  KeyEvents getKeyEvents(KeyStates& current_key_state);

  std::shared_ptr<Renderer> renderer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  std::shared_ptr<GameOptions> game_options_;
  std::shared_ptr<GameProcessor> game_frame_processor_;
  std::shared_ptr<LevelScreenProcessor> level_menu_processor_;
  std::shared_ptr<OptionScreenProcessor> option_menu_processor_;
  std::shared_ptr<KeyboardConfigProcessor> keyboard_config_processor_;
  std::shared_ptr<FrameProcessorInterface> active_processor_;
  KeyBindings key_bindings_;
  KeyStates key_states_;
  std::chrono::time_point<std::chrono::high_resolution_clock> frame_end_;
  Duration_ns single_frame_;
};

}  // namespace tetris_clone
