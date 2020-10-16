
#include "frame_processors/level_screen_processor.hpp"

#include "drawing_utils.hpp"
#include "frame_processor_interface.hpp"
#include "pixel_drawing_interface.hpp"

namespace tetris_clone {

ProgramFlowSignal LevelScreenProcessor::processKeyEvents(const KeyEvents& key_events) {
  auto& level = level_;
  if (options_selected_) {
    if (key_events.at(KeyAction::Left).pressed) {
      sample_player_->playSample("menu_blip");
      options_selected_ = false;
      return ProgramFlowSignal::FrameSuccess;
    }
    if (key_events.at(KeyAction::Start).pressed) {
      sample_player_->playSample("menu_select_02");
      return ProgramFlowSignal::OptionsScreen;
    }
  }

  if (key_events.at(KeyAction::Left).pressed && level > 0) {
    sample_player_->playSample("menu_blip");
    --level;
  }
  if (key_events.at(KeyAction::Right).pressed) {
    if (level == 4 || level == 9) {
      options_selected_ = true;
    } else {
      ++level;
    }
    sample_player_->playSample("menu_blip");
  }
  if (key_events.at(KeyAction::Down).pressed && level < 5) {
    sample_player_->playSample("menu_blip");
    level += 5;
  }
  if (key_events.at(KeyAction::Up).pressed && level >= 5) {
    sample_player_->playSample("menu_blip");
    level -= 5;
  }

  if (key_events.at(KeyAction::Start).pressed) {
    sample_player_->playSample("menu_select_01");
    plus_ten_levels_ = (key_events.at(KeyAction::RotateClockwise).held);
    return ProgramFlowSignal::StartGame;
  }
  return ProgramFlowSignal::FrameSuccess;
}

LevelScreenProcessor::LevelScreenProcessor(std::unique_ptr<PixelDrawingInterface>&& drawer,
                                           const std::shared_ptr<sound::SoundPlayer>& sample_player,
                                           const std::shared_ptr<SpriteProvider>& sprite_provider)
    : drawer_(std::move(drawer)),
      sample_player_(sample_player),
      sprite_provider_(sprite_provider),
      level_{},
      options_selected_{},
      plus_ten_levels_{},
      frame_counter_{std::make_unique<std::atomic_int>(0)} {}

void LevelScreenProcessor::renderMenu() const {
  if (*frame_counter_ == 0) {
    drawer_->drawSprite(0, 0, sprite_provider_->getSprite("a-type-background"));
  }

  // Clear the top area.
  drawer_->fillRect(30, 72, 190, 50, PixelDrawingInterface::BLACK());

  drawer_->drawString(157, 80, "OPTIONS");

  drawer_->drawString(72, 153, "JDMFX  1357428 30");
  drawer_->drawString(72, 169, "KORYAN 1273120 29");
  drawer_->drawString(72, 185, "JONAS  1245200 29");

  if (options_selected_) {
    const auto color =
        (*frame_counter_) % 4 ? PixelDrawingInterface::WHITE() : PixelDrawingInterface::BLACK();
    drawTriangleSelector(*drawer_, 148, 80, 7, color, false);
    drawTriangleSelector(*drawer_, 219, 80, 7, color, true);
    renderLevelSelector(-1);
  } else {
    renderLevelSelector(level_);
  }
}

void LevelScreenProcessor::renderLevelSelector(const int level) const {
  constexpr PixelDrawingInterface::Coords level_selector{51, 75};
  constexpr PixelDrawingInterface::Rect level_selector_size{83, 35};
  constexpr PixelDrawingInterface::Rect selector_size{16, 16};
  const PixelDrawingInterface::Color selector_color{252, 151, 56, 255};

  auto get_selector_coords = [&](const int level) -> PixelDrawingInterface::Coords {
    const int y = (level % 10 > 4) ? level_selector.y + selector_size.height : level_selector.y;
    return {(selector_size.width * (level % 5)) + level_selector.x + 1, y + 2};
  };

  // Draw the flashing selector, and then draw the levels on top with transparency.
  const auto coords = get_selector_coords(level);
  const auto color = *frame_counter_ % 4 ? selector_color : PixelDrawingInterface::BLACK();
  if (level >= 0) {
    drawer_->fillRect(coords, selector_size, color);
  }
  drawer_->drawSprite(level_selector, sprite_provider_->getSprite("levels-screen"));

  ++(*frame_counter_);
}

ProgramFlowSignal LevelScreenProcessor::processFrame(const KeyEvents& key_events) {
  auto signal = processKeyEvents(key_events);
  renderMenu();

  if (signal != ProgramFlowSignal::FrameSuccess) {
    *frame_counter_ = 0;
  }
  return signal;
}

int LevelScreenProcessor::getSelectedLevel() const {
  return level_ + (plus_ten_levels_ ? 10 : 0);
}

}  // namespace tetris_clone
