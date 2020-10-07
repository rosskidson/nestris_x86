
#include "option_screen_processor.hpp"

#include <memory>

#include "game_states.hpp"
#include "logging.hpp"
#include "option.hpp"

namespace tetris_clone {

OptionScreenProcessor::OptionScreenProcessor(
    const std::shared_ptr<Renderer>& renderer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player)
    : renderer_(renderer), sample_player_(sample_player), state_{} {
  state_.options["das_profile"] = std::make_unique<StringOption>(
      "DAS PROFILE", std::vector<std::string>{"NTSC", "PAL", "CUSTOM"});

  state_.options["refresh_frequency"] =  //
      std::make_unique<IntOption>("FREQUENCY (HZ)", 1, 99, 60);

  state_.options["das_initial_delay_frames"] =
      std::make_unique<IntOption>("DAS INITIAL DELAY", 0, 99, 16);

  state_.options["das_repeat_delay_frames"] =
      std::make_unique<IntOption>("DAS REPEAT DELAY", 1, 99, 6);

  state_.options["gravity_mode"] =
      std::make_unique<StringOption>("LEVEL GRAVITY", std::vector<std::string>{"NTSC", "PAL"});

  state_.options["hard_drop"] = std::make_unique<BoolOption>("HARD DROP", false);
  state_.options["wall_kick"] = std::make_unique<BoolOption>("WALL KICK", false);

  state_.options["show_das_meter"] = std::make_unique<BoolOption>("SHOW DAS METER", false);
  state_.options["show_das_chain"] = std::make_unique<BoolOption>("SHOW DAS CHAIN", false);
  state_.options["show_wall_charges"] = std::make_unique<BoolOption>("SHOW WALL CHARGES", false);
  state_.options["show_entry_delay"] = std::make_unique<BoolOption>("SHOW ENTRY DELAY", false);
  state_.options["show_controls"] = std::make_unique<BoolOption>("SHOW CONTROLS", false);

  state_.option_order = std::vector<std::string>({
      "das_profile",
      "refresh_frequency",
      "das_initial_delay_frames",
      "das_repeat_delay_frames",
      "gravity_mode",
      "hard_drop",
      "wall_kick",
      "show_das_meter",
      "show_das_chain",
      "show_wall_charges",
      "show_entry_delay",
      "show_controls",
  });

  // Missing options:
  //  HOLD
  //  STATSTICS MODE
  //  CONFIGURE KEYBOARD
  //  CONFIGURE CONTROLLER

  for (const auto& name : state_.option_order) {
    if (not state_.options.count(name)) {
      LOG_ERROR("Missing option in option map `" << name << "`.");
    }
  }
  for (const auto& [name, option] : state_.options) {
    if (std::find(state_.option_order.begin(), state_.option_order.end(), name) ==
        state_.option_order.end()) {
      LOG_ERROR("Missing option in option order list `" << name << "`.");
    }
  }
}

ProgramFlowSignal processKeyEvents(const KeyEvents& key_events,
                                   const sound::SoundPlayer& sample_player_,
                                   OptionState& menu_state) {
  if (key_events.at(KeyAction::Start).pressed) {
    sample_player_.playSample("menu_select_02");
    return ProgramFlowSignal::LevelSelectorScreen;
  }
  if (key_events.at(KeyAction::Up).pressed) {
    sample_player_.playSample("menu_blip");
    --menu_state.selected_index;
  }
  if (key_events.at(KeyAction::Down).pressed) {
    sample_player_.playSample("menu_blip");
    ++menu_state.selected_index;
  }
  if (key_events.at(KeyAction::Left).pressed) {
    sample_player_.playSample("menu_blip");
    menu_state.getSelectedOption().selectPrevOption();
  }
  if (key_events.at(KeyAction::Right).pressed) {
    sample_player_.playSample("menu_blip");
    menu_state.getSelectedOption().selectNextOption();
  }

  return ProgramFlowSignal::FrameSuccess;
}

ProgramFlowSignal OptionScreenProcessor::processFrame(const KeyEvents& key_events) {
  const auto signal = processKeyEvents(key_events, *sample_player_, state_);

  if(state_.option_order.at(state_.selected_index) == "das profile") {
    if(state_.getSelectedOption().getSelectedOptionText() == "NTSC") {
    }
  }
  renderer_->renderOptionScreen(state_);
  return signal;
}

}  // namespace tetris_clone
