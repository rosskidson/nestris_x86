
#pragma once

#include <memory>

#include "assets.hpp"
#include "drawers/pixel_drawing_interface.hpp"
#include "frame_processor_interface.hpp"
#include "sound.hpp"

namespace nestris_x86 {

class KeyboardConfigProcessor : public FrameProcessorInterface {
 public:
  KeyboardConfigProcessor(std::unique_ptr<PixelDrawingInterface>&& drawer,
                          const std::shared_ptr<sound::SoundPlayer>& sample_player,
                          std::shared_ptr<InputInterface>& input_interface,
                          const KeyBindings& initial_bindings);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

  KeyBindings getKeyBindings() const;
  void setKeyBindings(const KeyBindings& key_bindings);

 private:
  ProgramFlowSignal processKeyEvents(const KeyEvents& key_events);

  void renderKeyboardConfigScreen() const;

  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  KeyBindings key_bindings_;
  KeyBindings default_bindings_;
  KeyAction active_key_;
  int selector_idx_;
  bool wait_until_key_lifted_;
  bool keybinding_active_;
  std::unique_ptr<std::atomic_int> frame_counter_;

 protected:
  void clearScreen() const;

  std::unique_ptr<PixelDrawingInterface> drawer_;
  std::shared_ptr<InputInterface> input_ptr_;
};

}  // namespace nestris_x86

