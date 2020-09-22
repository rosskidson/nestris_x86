#pragma once

#include <memory>
#include <random>

#include "frame_processor_interface.hpp"
#include "game_states.hpp"
#include "rendering.hpp"
#include "sound.hpp"

namespace tetris_clone {

class LevelScreenProcessor : public FrameProcessorInterface {
 public:
  LevelScreenProcessor(const std::shared_ptr<Renderer>& renderer,
                       const std::shared_ptr<sound::SoundPlayer>& sample_player,
                       const std::shared_ptr<GameOptions>& options);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);


 private:
  std::shared_ptr<Renderer> renderer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<GameOptions> options_;
  MenuState state_;
};

}  // namespace tetris_clone
