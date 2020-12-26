
#include "frame_processors/gamepad_config_processor.hpp"

namespace tetris_clone {

using pdi = PixelDrawingInterface;

GamePadConfigProcessor::GamePadConfigProcessor(
    std::unique_ptr<PixelDrawingInterface>&& drawer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player,
    std::unique_ptr<InputInterface>&& input_interface, const KeyBindings& initial_bindings)
    : KeyboardConfigProcessor(std::move(drawer), sample_player, std::move(input_interface),
                              initial_bindings) {}

KeyBindings GamePadConfigProcessor::getDefaultBindings() {
  return getDefaultGamePadBindings(*input_ptr_);
}

ProgramFlowSignal GamePadConfigProcessor::processFrame(const KeyEvents& key_events) {
  renderWaitForControllerScreen();
  if (key_events.at(KeyAction::Start).pressed) {
    return ProgramFlowSignal::OptionsScreen;
  } else {
    return ProgramFlowSignal::FrameSuccess;
  }
}

void GamePadConfigProcessor::renderWaitForControllerScreen() const {
  KeyboardConfigProcessor::clearScreen();
  const pdi::Coords detect_msg_coords{30, 80};
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 00}, " NO CONTROLLER SUPPORT");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 10}, "       ON MacOS");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 30}, "A WORKAROUND IS TO USE A");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 40}, "   JOYSTICK REMAPPER     ");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 50}, " e.g. joystickmapper.com ");

}

}  // namespace tetris_clone
