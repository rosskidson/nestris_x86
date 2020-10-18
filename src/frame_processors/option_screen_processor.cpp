
#include "frame_processors/option_screen_processor.hpp"

#include <memory>

#include "das.hpp"
#include "drawing_utils.hpp"
#include "option.hpp"
#include "pixel_drawing_interface.hpp"
#include "tetris_type.hpp"
#include "utils/logging.hpp"

namespace tetris_clone {

namespace {

void setDasOptions(const int freq, const int inital_delay, const int repeat_delay,
                   const std::string& gravity, OptionScreenProcessor::OptionMap& option_map) {
  dynamic_cast<IntOption&>(*option_map.at("refresh_frequency")).setOption(freq);
  dynamic_cast<IntOption&>(*option_map.at("das_initial_delay_frames")).setOption(inital_delay);
  dynamic_cast<IntOption&>(*option_map.at("das_repeat_delay_frames")).setOption(repeat_delay);
  dynamic_cast<StringOption&>(*option_map.at("gravity_mode")).setOption(gravity);
}
}  // namespace

OptionScreenProcessor::OptionScreenProcessor(
    std::unique_ptr<PixelDrawingInterface>&& drawer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player,
    const std::shared_ptr<SpriteProvider>& sprite_provider)
    : drawer_(std::move(drawer)),
      sample_player_(sample_player),
      sprite_provider_(sprite_provider),
      options_{},
      option_order_{},
      selected_index_{},
      grey_out_das_options_{},
      frame_counter_{std::make_unique<std::atomic_int>(0)} {
  options_["configure_keyboard"] = std::make_unique<DummyOption>("CONFIGURE KEYBOARD");
  options_["configure_controller"] = std::make_unique<DummyOption>("CONFIGURE CONTROLLER");

  options_["das_profile"] = std::make_unique<StringOption>(
      "DAS PROFILE", std::vector<std::string>{"NTSC", "PAL", "CUSTOM"});

  options_["refresh_frequency"] =  //
      std::make_unique<IntOption>("FREQUENCY (HZ)", 1, 99, NTSC_FREQUENCY);

  options_["das_initial_delay_frames"] =
      std::make_unique<IntOption>("DAS INITIAL DELAY", 0, 99, 16);

  options_["das_repeat_delay_frames"] = std::make_unique<IntOption>("DAS REPEAT DELAY", 1, 99, 6);

  options_["gravity_mode"] =
      std::make_unique<StringOption>("LEVEL GRAVITY", std::vector<std::string>{"NTSC", "PAL"});

  options_["hard_drop"] = std::make_unique<BoolOption>("HARD DROP", false);
  options_["wall_kick"] = std::make_unique<BoolOption>("WALL KICK", false);

  options_["show_das_meter"] = std::make_unique<BoolOption>("SHOW DAS METER", false);
  options_["show_das_chain"] = std::make_unique<BoolOption>("SHOW DAS CHAIN", false);
  options_["show_wall_charges"] = std::make_unique<BoolOption>("SHOW WALL CHARGES", false);
  options_["show_entry_delay"] = std::make_unique<BoolOption>("SHOW ENTRY DELAY", false);
  options_["show_controls"] = std::make_unique<BoolOption>("SHOW CONTROLS", false);

  option_order_ = std::vector<std::string>({
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

  // Missing options_:
  //  HOLD
  //  STATSTICS MODE
  //  CONFIGURE KEYBOARD
  //  CONFIGURE CONTROLLER

  for (const auto& name : option_order_) {
    if (not options_.count(name)) {
      LOG_ERROR("Missing option in option map `" << name << "`.");
    }
  }
  for (const auto& [name, option] : options_) {
    if (std::find(option_order_.begin(), option_order_.end(), name) == option_order_.end()) {
      LOG_ERROR("Missing option in option order list `" << name << "`.");
    }
  }
}

ProgramFlowSignal OptionScreenProcessor::processKeyEvents(const KeyEvents& key_events) {
  auto& current_idx = selected_index_;
  if (key_events.at(KeyAction::Start).pressed) {
    sample_player_->playSample("menu_select_02");
    if (option_order_.at(selected_index_) == "configure_keyboard") {
      return ProgramFlowSignal::KeyboardConfigScreen;
    } else if (option_order_.at(selected_index_) == "configure_controller") {
      return ProgramFlowSignal::ControllerConfigScreen;
    } else {
      return ProgramFlowSignal::LevelSelectorScreen;
    }
  }
  if (key_events.at(KeyAction::Up).pressed) {
    sample_player_->playSample("menu_blip");
    current_idx = current_idx > 1 ? current_idx - 1 : 0;
  }
  if (key_events.at(KeyAction::Down).pressed) {
    sample_player_->playSample("menu_blip");
    current_idx =
        current_idx < option_order_.size() - 2 ? current_idx + 1 : option_order_.size() - 1;
  }
  if (key_events.at(KeyAction::Left).pressed) {
    sample_player_->playSample("menu_blip");
    getSelectedOption().selectPrevOption();
  }
  if (key_events.at(KeyAction::Right).pressed) {
    sample_player_->playSample("menu_blip");
    getSelectedOption().selectNextOption();
  }

  return ProgramFlowSignal::FrameSuccess;
}

std::vector<int> OptionScreenProcessor::renderOptions(const std::set<int>& spacers,
                                                      const int left_column, const int right_column,
                                                      const int first_row,
                                                      const bool grey_out_das_options_) const {
  drawer_->drawString(100, 17, "OPTIONS");
  const std::set<std::string> das_options{"refresh_frequency", "das_initial_delay_frames",
                                          "das_repeat_delay_frames", "gravity_mode"};

  std::vector<int> row_locations;
  int y_row = first_row;
  int counter = 0;
  for (const auto& name : option_order_) {
    const auto& option = options_.at(name);
    const auto color = grey_out_das_options_ && das_options.count(name)
                           ? PixelDrawingInterface::DARK_GREY()
                           : PixelDrawingInterface::WHITE();
    row_locations.push_back(y_row);
    const int indent = (dynamic_cast<DummyOption*>(option.get())) ? 15 : 0;
    drawer_->drawString(left_column + indent, y_row, option->getDisplayName(), color);
    drawer_->drawString(right_column, y_row, option->getSelectedOptionText(), color);
    y_row += 10;
    if (spacers.count(counter++)) {
      y_row += 5;
    }
  }
  return row_locations;
}

void OptionScreenProcessor::renderSelector(const int column_location,
                                           const std::vector<int>& row_locations) const {
  const auto color =
      (*frame_counter_)++ % 4 ? PixelDrawingInterface::WHITE() : PixelDrawingInterface::BLACK();
  constexpr int size = 7;
  const auto& option = getSelectedOption();
  if (isSelectedOptionDummy()) {
    drawTriangleSelector(*drawer_, 40, row_locations.at(selected_index_), size, color, true);
    drawTriangleSelector(*drawer_, 210, row_locations.at(selected_index_), size, color, false);
  }
  if (option.prevOptionAvailable()) {
    drawTriangleSelector(*drawer_, column_location, row_locations.at(selected_index_), size, color,
                         true);
  }
  if (option.nextOptionAvailable()) {
    drawTriangleSelector(*drawer_, column_location + 40, row_locations.at(selected_index_), size,
                         color, false);
  }
}

void OptionScreenProcessor::renderOptionScreen() const {
  // renderBackground("options-background");
  drawer_->drawSprite(0, 0, sprite_provider_->getSprite("options-background"));
  drawer_->fillRect(30, 30, 197, 180, PixelDrawingInterface::BLACK());
  constexpr int x_left_column = 32;
  constexpr int x_right_column = 180;
  constexpr int y_row_start = 40;

  std::set<int> spacers{1, 6, 8};
  const auto row_locations =
      renderOptions(spacers, x_left_column, x_right_column, y_row_start, grey_out_das_options_);
  renderSelector(x_right_column - 5, row_locations);
}

ProgramFlowSignal OptionScreenProcessor::processFrame(const KeyEvents& key_events) {
  const auto signal = processKeyEvents(key_events);

  if (options_.at("das_profile")->getSelectedOptionText() == "NTSC") {
    setDasOptions(NTSC_FREQUENCY, Das::NTSC_FULL_CHARGE,
                  Das::NTSC_FULL_CHARGE - Das::NTSC_MIN_CHARGE, "NTSC", options_);
    grey_out_das_options_ = true;
  } else if (options_.at("das_profile")->getSelectedOptionText() == "PAL") {
    setDasOptions(PAL_FREQUENCY, Das::PAL_FULL_CHARGE, Das::PAL_FULL_CHARGE - Das::PAL_MIN_CHARGE,
                  "PAL", options_);
    grey_out_das_options_ = true;
  } else {
    grey_out_das_options_ = false;
  }

  // Reset the frame counter if the processing has ended.
  if (signal != ProgramFlowSignal::FrameSuccess) {
    *frame_counter_ = 0;
  }

  renderOptionScreen();
  return signal;
}

}  // namespace tetris_clone
