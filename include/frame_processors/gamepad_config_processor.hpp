
#pragma once

#include <memory>

#include "keyboard_config_processor.hpp"

namespace tetris_clone {

class GamePadConfigProcessor : public KeyboardConfigProcessor {
 public:
  GamePadConfigProcessor(std::unique_ptr<PixelDrawingInterface>&& drawer,
                         const std::shared_ptr<sound::SoundPlayer>& sample_player,
                         std::unique_ptr<InputInterface>&& input_interface,
                         const KeyBindings& initial_bindings);

  KeyBindings getDefaultBindings() override;

  ProgramFlowSignal processFrame(const KeyEvents& key_events) override;
};

}  // namespace tetris_clone

