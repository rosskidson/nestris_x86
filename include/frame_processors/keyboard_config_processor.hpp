
#pragma once

#include <memory>

#include "assets.hpp"
#include "frame_processor_interface.hpp"
#include "pixel_drawing_interface.hpp"
#include "sound.hpp"

namespace tetris_clone {

class KeyboardConfigProcessor : public FrameProcessorInterface {
 public:
  KeyboardConfigProcessor(std::unique_ptr<PixelDrawingInterface>&& drawer,
                          const std::shared_ptr<sound::SoundPlayer>& sample_player);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

 private:
  ProgramFlowSignal processKeyEvents(const KeyEvents& key_events);

  void renderKeyboardConfigScreen();

  std::unique_ptr<PixelDrawingInterface> drawer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  KeyBindings key_bindings_;
  KeyAction active_key_;
  int key_counter_;

};

}  // namespace tetris_clone

