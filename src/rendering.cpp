#include "rendering.hpp"

#include <memory>
#include <set>

#include "assets.hpp"
#include "drawing_utils.hpp"
#include "frame_processors/game_processor.hpp"
#include "game_logic.hpp"
#include "game_states.hpp"
#include "key_defines.hpp"
#include "option.hpp"
#include "pixel_drawing_interface.hpp"
#include "utils/logging.hpp"

namespace tetris_clone {
using pdi = PixelDrawingInterface;

struct Coords {
  int x;
  int y;
};

struct Rect {
  int width;
  int height;
};

bool spriteValid(const olc::Sprite &sprite) {
  return sprite.height > 0 && sprite.width > 0;
}

bool loadBlockSprites(const SpriteProvider& sprite_provider,
                      std::vector<std::vector<std::unique_ptr<olc::Sprite>>> &block_sprites) {
  block_sprites.clear();
  block_sprites.resize(10);
  for (int level = 0; level < 10; ++level) {
    block_sprites[level].emplace_back(std::make_unique<olc::Sprite>(8, 8));
    for (int color = 0; color < 4; ++color) {
      std::stringstream ss;
      ss << "l" << level << "-c" << color;
      auto * sprite = sprite_provider.getSprite(ss.str())->Duplicate();
      std::unique_ptr<olc::Sprite> clone_ptr(sprite);
      block_sprites[level].emplace_back(std::move(clone_ptr));
      if (not spriteValid(*block_sprites.at(level).back())) {
        LOG_ERROR("Failed loading `" << ss.str() << "`.");
        return false;
      }
    }
  }
  return true;
}

Renderer::Renderer(std::unique_ptr<PixelDrawingInterface> &&drawer,
                   const std::shared_ptr<SpriteProvider> &sprite_provider,
                   const std::string &sprites_path)
    : drawer_(std::move(drawer)), sprite_provider_(sprite_provider), frame_counter_(0) {
  if (not loadBlockSprites(*sprite_provider_, block_sprites_)) {
    throw std::runtime_error("Failed loading block sprites.");
  }
}

void Renderer::renderText(const GameState<> &state) {
  constexpr Coords lines_pos{152, 16};
  constexpr Coords high_score_pos{192, 32};
  constexpr Coords score_pos{192, 56};
  constexpr Coords level_pos{208, 160};
  drawNumber(*drawer_, lines_pos.x, lines_pos.y, state.lines, 3);
  drawNumber(*drawer_, high_score_pos.x, high_score_pos.y, state.high_score, 6);
  drawNumber(*drawer_, score_pos.x, score_pos.y, state.score, 6);
  drawNumber(*drawer_, level_pos.x, level_pos.y, state.level, 2);

  constexpr Coords tetromino_counter_start{48, 88};
  for (int i = 0; i < 7; ++i) {
    drawNumber(*drawer_, tetromino_counter_start.x, tetromino_counter_start.y + (i * 16),
               state.tetromino_counts[i], 3, pdi::RED());
  }
}

void Renderer::renderNextTetromino(const Tetromino &next_tetromino, const int level) {
  auto get_next_tetromino_plotting_coords =
      [](const Tetromino &next_tetromino) -> std::tuple<int, int> {
    constexpr int start_x = 196;
    constexpr int start_y = 104;
    if (next_tetromino == Tetromino::Square) {
      return {start_x + 4, start_y + 8};
    } else if (next_tetromino == Tetromino::Line) {
      return {start_x - 4, start_y - 4};
    } else {
      return {start_x, start_y};
    }
  };

  // Clear existing tetromino.
  drawer_->fillRect(191, 112, 33, 15, pdi::BLACK());

  const auto tetromino = getTetrominoGrid(next_tetromino, 0);
  const auto [x, y] = get_next_tetromino_plotting_coords(next_tetromino);
  renderGrid(x, y, tetromino, level, 8, 8, getColor(next_tetromino));
}

void Renderer::renderEntryDelay(const bool delay_entry, const int x, const int y) {
  const auto delay_sprite = delay_entry ? "are-on" : "are-off";
  drawer_->drawSprite(x, y, sprite_provider_->getSprite(delay_sprite));
}

void Renderer::renderDasBar(const int das_counter, const Das &das_processor, const int x,
                            const int y) {
  const Coords das_box_pos{x, y};
  const Coords das_bar_pos{das_box_pos.x + 31, das_box_pos.y + 7};
  const int das_bar_length_pixels = 32;
  const int das_bar_width_pixels = 8;
  drawer_->drawSprite(das_box_pos.x, das_box_pos.y, sprite_provider_->getSprite("das-meter"));

  auto get_das_color = [&das_processor](const int &das) {
    const int das_min_charge = das_processor.getMinDasChargeCount();
    if (das >= das_min_charge) {
      return pdi::GREEN();
    } else if (das >= das_min_charge / 2) {
      return pdi::YELLOW();
    } else {
      return pdi::RED();
    };
  };

  auto get_das_bar_pixel_length = [&](const int das) {
    return das_processor.dasFullyCharged(das)
               ? das_bar_length_pixels
               : das_counter * (das_bar_length_pixels / das_processor.getFullDasChargeCount());
  };

  const auto color = get_das_color(das_counter);
  const auto das_bar_pixels = get_das_bar_pixel_length(das_counter);
  drawer_->fillRect(das_bar_pos.x, das_bar_pos.y, das_bar_pixels, das_bar_width_pixels, color);
}

void Renderer::renderControls(const GameState<> &state, const KeyEvents &key_events, const int x,
                              const int y) {
  const Coords controller_box_pos{x, y};
  const Coords arrow_left{controller_box_pos.x + 7, controller_box_pos.y + 13};
  const Coords arrow_right{controller_box_pos.x + 16, controller_box_pos.y + 13};
  const Coords arrow_up{controller_box_pos.x + 11, controller_box_pos.y + 9};
  const Coords arrow_down{controller_box_pos.x + 11, controller_box_pos.y + 17};
  const Coords a_button{controller_box_pos.x + 55, controller_box_pos.y + 15};
  const Coords b_button{controller_box_pos.x + 46, controller_box_pos.y + 15};
  const Coords start_button{controller_box_pos.x + 34, controller_box_pos.y + 17};

  drawer_->drawSprite(controller_box_pos.x, controller_box_pos.y,
                      sprite_provider_->getSprite("controller"));
  auto key_action_to_bool = [](const KeyEvent &key_event) {
    return key_event.held || key_event.pressed;
  };

  if (key_action_to_bool(key_events.at(KeyAction::Left))) {
    drawer_->fillRect(arrow_left.x, arrow_left.y, 4, 4, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::Right))) {
    drawer_->fillRect(arrow_right.x, arrow_right.y, 4, 4, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::Up))) {
    drawer_->fillRect(arrow_up.x, arrow_up.y, 5, 4, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::Down))) {
    drawer_->fillRect(arrow_down.x, arrow_down.y, 5, 4, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::RotateClockwise))) {
    drawer_->fillRect(a_button.x + 1, a_button.y, 4, 6, pdi::GREEN());
    drawer_->fillRect(a_button.x, a_button.y + 1, 6, 4, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::RotateAntiClockwise))) {
    drawer_->fillRect(b_button.x + 1, b_button.y, 4, 6, pdi::GREEN());
    drawer_->fillRect(b_button.x, b_button.y + 1, 6, 4, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::Start))) {
    drawer_->fillRect(start_button.x + 1, start_button.y, 4, 3, pdi::GREEN());
    drawer_->fillRect(start_button.x, start_button.y + 1, 6, 1, pdi::GREEN());
  }
}

void Renderer::renderBackground(const std::string background_sprite) {
  drawer_->drawSprite(0, 0, sprite_provider_->getSprite(background_sprite));
}

// This is a little hacky as it undermines the renderBackground function.
// TODO:: Send line clear animation info to the renderGameState fn.
void Renderer::doTetrisFlash(const int &line_clear_frame_number) {
  const auto &frame = line_clear_frame_number;
  if ((frame - 1) % 4 == 0) {
    drawer_->drawSprite(0, 0, sprite_provider_->getSprite("basic-field-flash"));
  } else {
    drawer_->drawSprite(0, 0, sprite_provider_->getSprite("basic-field-empty-black"));
  }
}

void Renderer::renderPaused() {
  constexpr Coords paused_pos{110, 116};
  drawer_->drawString(paused_pos.x, paused_pos.y, "PAUSED", pdi::WHITE());
}

bool new_game = true;

void Renderer::renderGameState(const GameState<> &state, const bool render_controls,
                               const bool render_das_bar, const bool render_entry_delay,
                               const KeyEvents &key_events, const Das &das_processor) {
  constexpr Coords grid_top_left{96, 40};
  constexpr Rect grid_size{80, 160};
  constexpr Coords counter_sprite_pos{13, 61};
  constexpr Coords das_box_pos{184, 175};
  constexpr Coords controller_box_pos{184, 196};
  constexpr Coords entry_delay_pos{193, 129};
  auto get_grid_for_render = [](const GameState<> &state) {
    if (entryDelay(state)) {
      return state.grid;
    } else {
      return addTetrominoToGrid(state.grid, state.active_tetromino);
    }
  };
  if (new_game) {
    renderBackground("basic-field-empty-black");
    new_game = false;
  }
  // Clear the field.
  drawer_->fillRect(grid_top_left.x, grid_top_left.y, grid_size.width, grid_size.height,
                    pdi::BLACK());

  drawer_->drawSprite(
      counter_sprite_pos.x, counter_sprite_pos.y,
      sprite_provider_->getSprite("l" + std::to_string(state.level % 10) + "-counts"));
  renderGrid(grid_top_left.x, grid_top_left.y, get_grid_for_render(state), state.level);
  renderNextTetromino(state.next_tetromino, state.level);
  renderText(state);
  if (render_controls) {
    renderControls(state, key_events, controller_box_pos.x, controller_box_pos.y);
  }
  if (render_das_bar) {
    renderDasBar(state.das_counter, das_processor, das_box_pos.x, das_box_pos.y);
  }
  if (render_entry_delay) {
    renderEntryDelay(entryDelay(state), entry_delay_pos.x, entry_delay_pos.y);
  }
  if (state.paused) {
    renderPaused();
  }
}

}  // namespace tetris_clone
