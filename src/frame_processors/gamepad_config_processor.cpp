
#include "frame_processors/gamepad_config_processor.hpp"
#include "olc_gamepad.hpp"
#include "utils/logging.hpp"

namespace tetris_clone {
GamePadConfigProcessor::GamePadConfigProcessor(
    std::unique_ptr<PixelDrawingInterface>&& drawer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player,
    std::unique_ptr<InputInterface>&& input_interface, const KeyBindings& initial_bindings)
    : KeyboardConfigProcessor(std::move(drawer), sample_player, std::move(input_interface),
                              initial_bindings) {}

KeyBindings GamePadConfigProcessor::getDefaultBindings() {
  return getDefaultGamePadBindings(*keyboard_input_);
}

ProgramFlowSignal GamePadConfigProcessor::processFrame(const KeyEvents& key_events){
  auto gamepad_raw_ptr = dynamic_cast<OlcGamePad*>(keyboard_input_.get());
  if(not gamepad_raw_ptr) {
    LOG_ERROR("Failed to cast gamepad config processor's input interface to a gamepad.");
    return ProgramFlowSignal::EndProgram;
  }
  if(not gamepad_raw_ptr->detectAndInit()) {
    LOG_INFO("Waiting to detect gamepad...");
    return ProgramFlowSignal::FrameSuccess;
  }
  return KeyboardConfigProcessor::processFrame(key_events);
}

}  // namespace tetris_clone
