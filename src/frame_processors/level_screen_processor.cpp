
#include "frame_processors/level_screen_processor.hpp"

#include "game_states.hpp"
#include "key_defines.hpp"
#include "sound.hpp"

namespace tetris_clone {

ProgramFlowSignal processKeyEvents(const KeyEvents& key_events, const sound::SoundPlayer& sample_player_,
                      MenuState& menu_state) {
  auto& level = menu_state.level;
  if (menu_state.options_selected) {
    if (key_events.at(KeyAction::Left).pressed) {
      sample_player_.playSample("menu_blip");
      menu_state.options_selected = false;
      return ProgramFlowSignal::FrameSuccess;
    }
    if(key_events.at(KeyAction::Start).pressed) {
      sample_player_.playSample("menu_select_02");
      return ProgramFlowSignal::OptionsScreen;
    }
  }

  if (key_events.at(KeyAction::Left).pressed && level > 0) {
    sample_player_.playSample("menu_blip");
    --level;
  }
  if (key_events.at(KeyAction::Right).pressed) {
    if (level == 4 || level == 9) {
      menu_state.options_selected = true;
    } else {
      ++level;
    }
    sample_player_.playSample("menu_blip");
  }
  if (key_events.at(KeyAction::Down).pressed && level < 5) {
    sample_player_.playSample("menu_blip");
    level += 5;
  }
  if (key_events.at(KeyAction::Up).pressed && level >= 5) {
    sample_player_.playSample("menu_blip");
    level -= 5;
  }

  if (key_events.at(KeyAction::Start).pressed) {
    sample_player_.playSample("menu_select_01");
    menu_state.plus_ten_levels = (key_events.at(KeyAction::RotateClockwise).held);
    return ProgramFlowSignal::StartGame;
  }
  return ProgramFlowSignal::FrameSuccess;
}

LevelScreenProcessor::LevelScreenProcessor(const std::shared_ptr<Renderer>& renderer,
                                           const std::shared_ptr<sound::SoundPlayer>& sample_player)
    : renderer_(renderer), sample_player_(sample_player), state_{} {}

ProgramFlowSignal LevelScreenProcessor::processFrame(const KeyEvents& key_events) {
  auto signal = processKeyEvents(key_events, *sample_player_, state_);

  renderer_->renderMenu(state_);

  return signal;
}

int LevelScreenProcessor::getSelectedLevel() {
  return state_.level + (state_.plus_ten_levels ? 10 : 0);
}

}  // namespace tetris_clone
