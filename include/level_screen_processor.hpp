#pragma once

#include <memory>

#include "frame_processor_interface.hpp"
#include "game_states.hpp"
#include "rendering.hpp"
#include "sound.hpp"

namespace tetris_clone {

class LevelScreenProcessor : public FrameProcessorInterface {
 public:
  LevelScreenProcessor(const std::shared_ptr<Renderer>& renderer,
                       const std::shared_ptr<sound::SoundPlayer>& sample_player);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

  int getSelectedLevel();

 private:
  std::shared_ptr<Renderer> renderer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  MenuState state_;
};

}  // namespace tetris_clone
