
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
                          const std::shared_ptr<sound::SoundPlayer>& sample_player,
                          std::unique_ptr<InputInterface>&& input_interface);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

 private:
  ProgramFlowSignal processKeyEvents(const KeyEvents& key_events);

  void renderKeyboardConfigScreen();

  std::unique_ptr<PixelDrawingInterface> drawer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  std::unique_ptr<InputInterface> keyboard_input_;
  KeyBindings key_bindings_;
  KeyAction active_key_;
  int key_counter_;
  bool wait_until_key_lifted_;
  bool keybinding_finished_;

};

}  // namespace tetris_clone

