
#include "frame_processors/gamepad_config_processor.hpp"

#include <iso646.h>

#include "drawers/pixel_drawing_interface.hpp"
#include "input_devices/olc_gamepad.hpp"
#include "utils/logging.hpp"

namespace nestris_x86 {

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
  auto gamepad_raw_ptr = dynamic_cast<OlcGamePad*>(input_ptr_.get());
  if (not gamepad_raw_ptr) {
    LOG_ERROR("Failed to cast gamepad config processor's input interface to a gamepad.");
    return ProgramFlowSignal::EndProgram;
  }
  if (not gamepad_raw_ptr->detectAndInit()) {
#if __APPLE__
    renderMacOsSupportUnavailable();
#else
    renderWaitForControllerScreen();
#endif
    if (key_events.at(KeyAction::Start).pressed) {
      return ProgramFlowSignal::OptionsScreen;
    } else {
      return ProgramFlowSignal::FrameSuccess;
    }
  }
  return KeyboardConfigProcessor::processFrame(key_events);
}

void GamePadConfigProcessor::renderWaitForControllerScreen() const {
  KeyboardConfigProcessor::clearScreen();
  const pdi::Coords detect_msg_coords{30, 80};
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 00}, " NO CONTROLLER DETECTED");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 30}, "   PRESS ANY KEY ON");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 40}, "   THE CONTROLLER TO");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 50}, "      ACTIVATE...");
}

void GamePadConfigProcessor::renderMacOsSupportUnavailable() const {
  KeyboardConfigProcessor::clearScreen();
  const pdi::Coords detect_msg_coords{30, 80};
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 00}, " NO CONTROLLER SUPPORT");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 10}, "       ON MacOS");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 30}, "A WORKAROUND IS TO USE A");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 40}, "   JOYSTICK REMAPPER     ");
  drawer_->drawString(detect_msg_coords + pdi::Coords{0, 50}, " e.g. joystickmapper.com ");

}

}  // namespace nestris_x86
