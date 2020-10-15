
#include "frame_processors/keyboard_config_processor.hpp"

#include "frame_processor_interface.hpp"
#include "pixel_drawing_interface.hpp"
#include "tetris.hpp"

namespace tetris_clone {

KeyboardConfigProcessor::KeyboardConfigProcessor(
    std::unique_ptr<PixelDrawingInterface>&& drawer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player)
    : drawer_(std::move(drawer)), sample_player_(sample_player) {}

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
    drawer_->drawString(x_right_column, y_row, keyToString(key));
    y_row += 10;
  }
  drawer_->drawString(x_left_column, y_row, keyActionToString(active_key_));
  drawer_->drawString(x_right_column, y_row, "???");
}

ProgramFlowSignal KeyboardConfigProcessor::processFrame(const KeyEvents& key_events) {
  const auto signal = processKeyEvents(key_events);

  for (int i = 0; i < static_cast<int>(KeyAction::COUNT); ++i) {
    const auto& key_action = static_cast<KeyAction>(i);
    if (not key_bindings_.count(key_action)) {
      active_key_ = key_action;
      break;
    }
  }

  renderKeyboardConfigScreen();

  return signal;
}

}  // namespace tetris_clone
