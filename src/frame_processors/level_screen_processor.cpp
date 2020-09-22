
#include "level_screen_processor.hpp"

#include "game_states.hpp"
#include "key_defines.hpp"
#include "sound.hpp"

namespace tetris_clone {

GameOptions getGameOptions(const MenuState& menu_state) {
  const int level = menu_state.plus_ten_levels ? menu_state.level + 10 : menu_state.level;
  return GameOptions{level, 16, 10, 60, true, StatisticsMode::classic};
}

void processKeyEvents(const KeyEvents& key_events, const sound::SoundPlayer& sample_player_,
                      MenuState& menu_state) {
  auto& level = menu_state.level;
  if (key_events.at(KeyAction::Left).pressed && level > 0) {
    sample_player_.playSample("menu_blip");
    --level;
  }
  if (key_events.at(KeyAction::Right).pressed && level < 9) {
    sample_player_.playSample("menu_blip");
    ++level;
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
    menu_state.plus_ten_levels = (key_events.at(KeyAction::RotateLeft).held);
    menu_state.start_game = true;
  }
}

LevelScreenProcessor::LevelScreenProcessor(const std::shared_ptr<Renderer>& renderer,
                                           const std::shared_ptr<sound::SoundPlayer>& sample_player,
                                           const std::shared_ptr<GameOptions>& options)
    : renderer_(renderer), sample_player_(sample_player), options_{options}, state_{} {}

ProgramFlowSignal LevelScreenProcessor::processFrame(const KeyEvents& key_events) {
  processKeyEvents(key_events, *sample_player_, state_);

  renderer_->renderMenu(state_);

  if (state_.start_game) {
    state_.start_game = false;
    *options_ = getGameOptions(state_);
    return ProgramFlowSignal::StartGame;
  } else {
    return ProgramFlowSignal::FrameSuccess;
  }
}

}  // namespace tetris_clone
