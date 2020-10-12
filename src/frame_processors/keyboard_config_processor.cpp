
#include "keyboard_config_processor.hpp"
#include "frame_processor_interface.hpp"
#include "tetris.hpp"

namespace tetris_clone {

KeyboardConfigProcessor::KeyboardConfigProcessor(
    const std::shared_ptr<Renderer>& renderer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player)
    : renderer_(renderer), sample_player_(sample_player), state_{} {}

//ProgramFlowSignal processKeyEvents(const KeyEvents& key_events,
//                                   const sound::SoundPlayer& sample_player_,
//                                   KeyboardConfigState& state) {
//  return ProgramFlowSignal::FrameSuccess;
//}

olc::Key detectPressedKey() {


}


ProgramFlowSignal KeyboardConfigProcessor::processFrame(const KeyEvents& key_events) {

  //const auto signal = processKeyEvents(key_events, *sample_player_, state_);
  for(int i = 0; i < static_cast<int>(KeyAction::COUNT); ++i) {
    const auto& key_action = static_cast<KeyAction>(i);
    if(not state_.key_bindings.count(key_action)) {
      state_.active_key = key_action;
      break;
    }
  }

  renderer_->renderKeyboardConfigScreen(state_);

  return ProgramFlowSignal::FrameSuccess;
}

}  // namespace tetris_clone
