#pragma once

#include <chrono>
#include <memory>

#include "key_defines.hpp"
#include "olcPixelGameEngine.h"
#include "rendering.hpp"
#include "sound.hpp"
#include "frame_processor_interface.hpp"

namespace tetris_clone {

class TetrisClone : public olc::PixelGameEngine {
 public:
  TetrisClone(const int start_level = 0);

  bool OnUserCreate() override;

  bool OnUserUpdate(float fElapsedTime) override;

 private:
  void sleepUntilNextFrame();

  KeyEvents getKeyEvents(KeyStates& current_key_state);

  std::shared_ptr<Renderer> renderer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<FrameProcessorInterface> game_frame_processor_;
  std::shared_ptr<FrameProcessorInterface> level_menu_processor_;
  std::shared_ptr<FrameProcessorInterface> active_processor_;
  bool menu_;
  KeyStates key_states_;
  KeyBindings key_bindings_;
  std::chrono::time_point<std::chrono::high_resolution_clock> frame_end_;
};

}  // namespace tetris_clone
