
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
                          std::unique_ptr<InputInterface>&& input_interface,
                          const KeyBindings& initial_bindings);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

  KeyBindings getKeyBindings() const;

 private:
  ProgramFlowSignal processKeyEvents(const KeyEvents& key_events);

  void renderKeyboardConfigScreen() const;

  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  KeyBindings key_bindings_;
  KeyAction active_key_;
  int selector_idx_;
  bool wait_until_key_lifted_;
  bool keybinding_active_;
  std::unique_ptr<std::atomic_int> frame_counter_;

 protected:
  virtual KeyBindings getDefaultBindings();
  void clearScreen() const;

  std::unique_ptr<PixelDrawingInterface> drawer_;
  std::unique_ptr<InputInterface> input_ptr_;
};

}  // namespace tetris_clone

