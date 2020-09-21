
#include "level_screen_processor.hpp"

#include "key_defines.hpp"

namespace tetris_clone {

LevelScreenProcessor::LevelScreenProcessor(const std::shared_ptr<Renderer>& renderer,
                                           const std::shared_ptr<sound::SoundPlayer>& sample_player)
    : renderer_(renderer), sample_player_(sample_player), state_{} {}

ProgramFlowSignal LevelScreenProcessor::processFrame(const KeyEvents& key_events) {
  renderer_->renderMenu(state_);
  if (key_events.at(KeyAction::Start).pressed) {
    return ProgramFlowSignal::StartGame;
  }

  return ProgramFlowSignal::FrameSuccess;
}
}  // namespace tetris_clone
