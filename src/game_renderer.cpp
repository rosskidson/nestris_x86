#include "game_renderer.hpp"

#include <memory>
#include <set>

#include "assets.hpp"
#include "drawing_utils.hpp"
#include "game_logic.hpp"
#include "utils/logging.hpp"

namespace tetris_clone {
using pdi = PixelDrawingInterface;

bool spriteValid(const olc::Sprite &sprite) {
  return sprite.height > 0 && sprite.width > 0;
}

bool loadBlockSprites(const SpriteProvider &sprite_provider,
                      std::vector<std::vector<std::unique_ptr<olc::Sprite>>> &block_sprites) {
  block_sprites.clear();
  block_sprites.resize(10);
  for (int level = 0; level < 10; ++level) {
    block_sprites[level].emplace_back(std::make_unique<olc::Sprite>(8, 8));
    for (int color = 0; color < 4; ++color) {
      std::stringstream ss;
      ss << "l" << level << "-c" << color;
      auto *sprite = sprite_provider.getSprite(ss.str())->Duplicate();
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

GameRenderer::GameRenderer(std::unique_ptr<PixelDrawingInterface> &&drawer,
                           const std::shared_ptr<SpriteProvider> &sprite_provider,
                           const std::string &sprites_path)
    : drawer_(std::move(drawer)),
      sprite_provider_(sprite_provider),
      block_sprites_{},
      background_rendered_{} {
  if (not loadBlockSprites(*sprite_provider_, block_sprites_)) {
    throw std::runtime_error("Failed loading block sprites.");
  }
}

void GameRenderer::startNewGame() {
  background_rendered_ = false;
}

olc::Sprite *GameRenderer::getBlockSprite(const int level, const int color) const try {
  return block_sprites_.at(level % 10).at(color).get();
} catch (const std::out_of_range &e) {
  LOG_ERROR("Block sprite not loaded. Level[" << level << "] Color[" << color << "]");
  throw;
}

void GameRenderer::renderText(const GameState<> &state) const {
  constexpr pdi::Coords lines_pos{152, 16};
  constexpr pdi::Coords high_score_pos{192, 32};
  constexpr pdi::Coords score_pos{192, 56};
  constexpr pdi::Coords level_pos{208, 160};
  drawNumber(*drawer_, lines_pos, state.lines, 3);
  drawNumber(*drawer_, high_score_pos, state.high_score, 6);
  drawNumber(*drawer_, score_pos, state.score, 6);
  drawNumber(*drawer_, level_pos, state.level, 2);

  constexpr pdi::Coords tetromino_counter_start{48, 88};
  for (int i = 0; i < 7; ++i) {
    drawNumber(*drawer_, tetromino_counter_start.x, tetromino_counter_start.y + (i * 16),
               state.tetromino_counts[i], 3, pdi::RED());
  }
}

void GameRenderer::renderNextTetromino(const Tetromino &next_tetromino, const int level) const {
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

void GameRenderer::renderEntryDelay(const bool delay_entry,
                                    const PixelDrawingInterface::Coords &position) const {
  const auto delay_sprite = delay_entry ? "are-on" : "are-off";
  drawer_->drawSprite(position, sprite_provider_->getSprite(delay_sprite));
}

void GameRenderer::renderDasBar(const int das_counter, const Das &das_processor,
                                const pdi::Coords &das_box_pos) const {
  const pdi::Coords das_bar_pos{das_box_pos.x + 31, das_box_pos.y + 7};
  const int das_bar_length_pixels = 32;
  const int das_bar_width_pixels = 8;
  drawer_->drawSprite(das_box_pos, sprite_provider_->getSprite("das-meter"));

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

void GameRenderer::renderControls(const GameState<> &state, const KeyEvents &key_events,
                                  const pdi::Coords &control_position) const {
  const auto x = control_position.x;
  const auto y = control_position.y;
  const pdi::Coords controller_box_pos{x, y};
  const pdi::Coords arrow_left{controller_box_pos.x + 7, controller_box_pos.y + 13};
  const pdi::Coords arrow_right{controller_box_pos.x + 16, controller_box_pos.y + 13};
  const pdi::Coords arrow_up{controller_box_pos.x + 11, controller_box_pos.y + 9};
  const pdi::Coords arrow_down{controller_box_pos.x + 11, controller_box_pos.y + 17};
  const pdi::Coords a_button{controller_box_pos.x + 55, controller_box_pos.y + 15};
  const pdi::Coords b_button{controller_box_pos.x + 46, controller_box_pos.y + 15};
  const pdi::Coords start_button{controller_box_pos.x + 34, controller_box_pos.y + 17};

  drawer_->drawSprite(controller_box_pos, sprite_provider_->getSprite("controller"));
  auto key_action_to_bool = [](const KeyEvent &key_event) {
    return key_event.held || key_event.pressed;
  };

  if (key_action_to_bool(key_events.at(KeyAction::Left))) {
    drawer_->fillRect(arrow_left, {4, 4}, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::Right))) {
    drawer_->fillRect(arrow_right, {4, 4}, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::Up))) {
    drawer_->fillRect(arrow_up, {5, 4}, pdi::GREEN());
  }
  if (key_action_to_bool(key_events.at(KeyAction::Down))) {
    drawer_->fillRect(arrow_down, {5, 4}, pdi::GREEN());
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

void GameRenderer::renderBackground() {
  if (not background_rendered_) {
    drawer_->drawSprite(0, 0, sprite_provider_->getSprite("basic-field-empty-black"));
    background_rendered_ = true;
  }
}

void GameRenderer::doTetrisFlash(const int &line_clear_frame_number) const {
  const auto &frame = line_clear_frame_number;
  if ((frame - 1) % 4 == 0) {
    drawer_->drawSprite(0, 0, sprite_provider_->getSprite("basic-field-flash"));
  } else {
    drawer_->drawSprite(0, 0, sprite_provider_->getSprite("basic-field-empty-black"));
  }
}

void GameRenderer::renderPaused() const {
  constexpr pdi::Coords paused_pos{110, 116};
  drawer_->drawString(paused_pos, "PAUSED", pdi::WHITE());
}

void GameRenderer::renderGameState(const GameState<> &state, const bool render_controls,
                                   const bool render_das_bar, const bool render_entry_delay,
                                   const KeyEvents &key_events, const Das &das_processor) {
  constexpr pdi::Coords grid_top_left{96, 40};
  constexpr pdi::Rect grid_size{80, 160};
  constexpr pdi::Coords counter_sprite_pos{13, 61};
  constexpr pdi::Coords das_box_pos{184, 175};
  constexpr pdi::Coords controller_box_pos{184, 196};
  constexpr pdi::Coords entry_delay_pos{193, 129};
  auto get_grid_for_render = [](const GameState<> &state) {
    if (entryDelay(state)) {
      return state.grid;
    } else {
      return addTetrominoToGrid(state.grid, state.active_tetromino);
    }
  };
  renderBackground();
  // Clear the field.
  drawer_->fillRect(grid_top_left, grid_size, pdi::BLACK());

  drawer_->drawSprite(counter_sprite_pos, sprite_provider_->getSprite(
                                              "l" + std::to_string(state.level % 10) + "-counts"));
  renderGrid(grid_top_left.x, grid_top_left.y, get_grid_for_render(state), state.level);
  renderNextTetromino(state.next_tetromino, state.level);
  renderText(state);
  if (render_controls) {
    renderControls(state, key_events, controller_box_pos);
  }
  if (render_das_bar) {
    renderDasBar(state.das_counter, das_processor, das_box_pos);
  }
  if (render_entry_delay) {
    renderEntryDelay(entryDelay(state), entry_delay_pos);
  }
  if (state.paused) {
    renderPaused();
  }
}

}  // namespace tetris_clone
