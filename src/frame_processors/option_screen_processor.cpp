
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
  state_.options["configure_keyboard"] = std::make_unique<DummyOption>("CONFIGURE KEYBOARD");
  state_.options["configure_controller"] = std::make_unique<DummyOption>("CONFIGURE CONTROLLER");

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
      "configure_keyboard",
      "configure_controller",
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
  auto& current_idx = menu_state.selected_index;
  if (key_events.at(KeyAction::Start).pressed) {
    sample_player_.playSample("menu_select_02");
    if (menu_state.option_order.at(menu_state.selected_index) == "configure_keyboard") {
      return ProgramFlowSignal::KeyboardConfigScreen;
    } else if (menu_state.option_order.at(menu_state.selected_index) == "configure_controller") {
      return ProgramFlowSignal::ControllerConfigScreen;
    } else {
      return ProgramFlowSignal::LevelSelectorScreen;
    }
  }
  if (key_events.at(KeyAction::Up).pressed) {
    sample_player_.playSample("menu_blip");
    current_idx = current_idx > 1 ? current_idx - 1 : 0;
  }
  if (key_events.at(KeyAction::Down).pressed) {
    sample_player_.playSample("menu_blip");
    current_idx = current_idx < menu_state.option_order.size() - 2
                      ? current_idx + 1
                      : menu_state.option_order.size() - 1;
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

void setDasOptions(const int freq, const int inital_delay, const int repeat_delay,
                   const std::string& gravity, OptionState::OptionMap& option_map) {
  dynamic_cast<IntOption&>(*option_map.at("refresh_frequency")).setOption(freq);
  dynamic_cast<IntOption&>(*option_map.at("das_initial_delay_frames")).setOption(inital_delay);
  dynamic_cast<IntOption&>(*option_map.at("das_repeat_delay_frames")).setOption(repeat_delay);
  dynamic_cast<StringOption&>(*option_map.at("gravity_mode")).setOption(gravity);
}

ProgramFlowSignal OptionScreenProcessor::processFrame(const KeyEvents& key_events) {
  const auto signal = processKeyEvents(key_events, *sample_player_, state_);

  if (state_.options.at("das_profile")->getSelectedOptionText() == "NTSC") {
    setDasOptions(60, 16, 6, "NTSC", state_.options);
    state_.grey_out_das_options = true;
  } else if (state_.options.at("das_profile")->getSelectedOptionText() == "PAL") {
    setDasOptions(50, 12, 4, "PAL", state_.options);
    state_.grey_out_das_options = true;
  } else {
    state_.grey_out_das_options = false;
  }

  renderer_->renderOptionScreen(state_);
  return signal;
}

}  // namespace tetris_clone
