#pragma once

#include <memory>

#include "frame_processor_interface.hpp"
#include "game_states.hpp"
#include "rendering.hpp"
#include "sound.hpp"

namespace tetris_clone {

class OptionScreenProcessor : public FrameProcessorInterface {
 public:
  OptionScreenProcessor(const std::shared_ptr<Renderer>& renderer,
                        const std::shared_ptr<sound::SoundPlayer>& sample_player);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

 private:
  std::shared_ptr<Renderer> renderer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  OptionState state_;
};

}  // namespace tetris_clone
