
#include "frame_processors/keyboard_config_processor.hpp"

#include "pixel_drawing_interface.hpp"

namespace tetris_clone {

KeyboardConfigProcessor::KeyboardConfigProcessor(
    std::unique_ptr<PixelDrawingInterface>&& drawer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player,
    std::unique_ptr<InputInterface>&& input_interface)
    : drawer_(std::move(drawer)),
      sample_player_(sample_player),
      keyboard_input_(std::move(input_interface)),
      key_bindings_{},
      active_key_{},
      key_counter_{},
      wait_until_key_lifted_{true},
      keybinding_finished_{} {}

ProgramFlowSignal KeyboardConfigProcessor::processKeyEvents(const KeyEvents& key_events) {
  return ProgramFlowSignal::FrameSuccess;
}

void KeyboardConfigProcessor::renderKeyboardConfigScreen() {
  drawer_->fillRect(30, 30, 197, 180, PixelDrawingInterface::BLACK());
  constexpr int x_left_column = 32;
  constexpr int x_right_column = 180;
  int y_row = 40;
  for (const auto& [action, key] : key_bindings_) {
    drawer_->drawString(x_left_column, y_row, keyActionToString(action));
    drawer_->drawString(x_right_column, y_row, keyboard_input_->keyCodeToStr(key));
    y_row += 10;
  }
  if (active_key_ < KeyAction::COUNT) {
    drawer_->drawString(x_left_column, y_row, keyActionToString(active_key_));
    drawer_->drawString(x_right_column, y_row, "???");
  }
}

ProgramFlowSignal KeyboardConfigProcessor::processFrame(const KeyEvents& key_events) {
  if (wait_until_key_lifted_) {
    wait_until_key_lifted_ = (keyboard_input_->getPressedKey() > 0) ? true : false;
    return ProgramFlowSignal::FrameSuccess;
  }
  // const auto signal = processKeyEvents(key_events);

  if (not keybinding_finished_) {
    const auto pressed_key = keyboard_input_->getPressedKey();
    if (pressed_key != keyboard_input_->lookupKeyCode("NONE")) {
      key_bindings_[active_key_] = pressed_key;
      wait_until_key_lifted_ = true;
      active_key_ = static_cast<KeyAction>(static_cast<int>(active_key_) + 1);
      if (active_key_ == KeyAction::COUNT) {
        keybinding_finished_ = true;
      }
    }
  }

  renderKeyboardConfigScreen();

  return ProgramFlowSignal::FrameSuccess;
}

}  // namespace tetris_clone
