
#include "option_screen_processor.hpp"

#include "game_states.hpp"

namespace tetris_clone {

OptionScreenProcessor::OptionScreenProcessor(
    const std::shared_ptr<Renderer>& renderer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player)
    : renderer_(renderer), sample_player_(sample_player), state_{} {}

ProgramFlowSignal processKeyEvents(const KeyEvents& key_events,
                                   const sound::SoundPlayer& sample_player_,
                                   OptionState& menu_state) {
  if (key_events.at(KeyAction::Start).pressed) {
    sample_player_.playSample("menu_select_02");
    return ProgramFlowSignal::LevelSelectorScreen;
  }

  return ProgramFlowSignal::FrameSuccess;
}

ProgramFlowSignal OptionScreenProcessor::processFrame(const KeyEvents& key_events) {
  const auto signal = processKeyEvents(key_events, *sample_player_, state_);
  renderer_->renderOptionScreen(state_);
  return signal;
}

}  // namespace tetris_clone
