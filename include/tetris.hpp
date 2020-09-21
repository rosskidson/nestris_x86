#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "game_states.hpp"
#include "key_defines.hpp"
#include "olcPixelGameEngine.h"
#include "rendering.hpp"
#include "sound.hpp"
#include "tetromino.hpp"
#include "frame_processor_interface.hpp"

namespace tetris_clone {

class TetrisClone : public olc::PixelGameEngine {
 public:
  TetrisClone(const int start_level = 0);

  bool OnUserCreate() override;

  bool OnUserUpdate(float fElapsedTime) override;

 private:
  ProgramFlowSignal runMenuSingleFrame(const KeyEvents& key_events);
  void sleepUntilNextFrame();

  KeyEvents getKeyEvents(KeyStates& current_key_state);

  std::shared_ptr<Renderer> renderer_;
  std::unique_ptr<FrameProcessorInterface> game_frame_processor_;
  MenuState menu_state_;
  bool menu_;
  KeyStates key_states_;
  KeyBindings key_bindings_;
  std::chrono::time_point<std::chrono::high_resolution_clock> frame_end_;
};

}  // namespace tetris_clone
