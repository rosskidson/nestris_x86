#include "rendering.hpp"

#include "assets.hpp"
#include "game_logic.hpp"
#include "logging.hpp"
#include "olcPixelGameEngine.h"

namespace tetris_clone {

struct Coords {
  int x;
  int y;
};

struct Rect {
  int width;
  int height;
};

Renderer::Renderer(olc::PixelGameEngine &render_engine, const std::string &sprites_path)
    : render_engine_ref_(render_engine), frame_counter_(0) {
  if (not loadBlockSprites(sprites_path, block_sprites_)) {
    throw std::runtime_error("Failed loading block sprites.");
  }
  if (not loadSprites(sprites_path, sprite_map_)) {
    throw std::runtime_error("Failed loading sprites.");
  }
}

void Renderer::drawNumber(const int x, const int y, const int num, const int pad,
                          const olc::Pixel color) {
  constexpr int TEXT_WIDTH_PX = 8;
  render_engine_ref_.FillRect(x, y, TEXT_WIDTH_PX * pad, TEXT_WIDTH_PX, olc::BLACK);
  std::stringstream ss;
  ss << std::setw(pad) << std::setfill('0') << num;
  render_engine_ref_.DrawString(x, y, ss.str(), color);
}

void Renderer::renderText(const GameState<> &state) {
  constexpr Coords lines_pos{152, 16};
  constexpr Coords high_score_pos{192, 32};
  constexpr Coords score_pos{192, 56};
  constexpr Coords level_pos{208, 160};
  drawNumber(lines_pos.x, lines_pos.y, state.lines, 3);
  drawNumber(high_score_pos.x, high_score_pos.y, state.high_score, 6);
  drawNumber(score_pos.x, score_pos.y, state.score, 6);
  drawNumber(level_pos.x, level_pos.y, state.level, 2);

  constexpr Coords tetromino_counter_start{48, 88};
  for (int i = 0; i < 7; ++i) {
    drawNumber(tetromino_counter_start.x, tetromino_counter_start.y + (i * 16),
               state.tetromino_counts[i], 3, olc::RED);
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
  render_engine_ref_.FillRect(191, 112, 33, 15, olc::BLACK);

  const auto tetromino = getTetrominoGrid(next_tetromino, 0);
  const auto [x, y] = get_next_tetromino_plotting_coords(next_tetromino);
  renderGrid(x, y, tetromino, level, 8, 8, getColor(next_tetromino));
}

void Renderer::renderDebug(const GameState<> &state, const KeyEvents &key_events) {
  auto get_das_color = [](const int &das) {
    if (das >= DAS_MIN_CHARGE) {
      return olc::GREEN;
    } else if (das >= DAS_MIN_CHARGE / 2) {
      return olc::YELLOW;
    } else {
      return olc::RED;
    };
  };
  const auto color = get_das_color(state.das_counter);
  render_engine_ref_.FillRect(200, 180, 3 * 16, 6, olc::BLACK);
  render_engine_ref_.FillRect(200, 180, 3 * state.das_counter, 6, color);

  auto key_action_to_bool = [](const KeyEvent &key_event) {
    return key_event.held || key_event.pressed;
  };

  auto fill_circle = [&](const int x, const int y, const int radius, const bool signal,
                         const olc::Pixel &on_color, const olc::Pixel &off_color) {
    auto color = signal ? on_color : off_color;
    render_engine_ref_.FillCircle(x, y, radius, color);
  };

  // fill_circle(194, 183, 3, state.das_reset_signal, olc::RED, olc::GREEN);

  render_engine_ref_.DrawString(190, 189, "ARE:", olc::WHITE);
  fill_circle(226, 192, 3, entryDelay(state), olc::GREEN, olc::RED);

  render_engine_ref_.DrawString(204, 199, "<  >", olc::WHITE);
  fill_circle(196, 202, 4, key_action_to_bool(key_events.at(KeyAction::Left)), olc::GREEN,
              olc::BLACK);
  fill_circle(242, 201, 4, key_action_to_bool(key_events.at(KeyAction::Right)), olc::GREEN,
              olc::BLACK);
  render_engine_ref_.DrawString(204, 207, "A  B", olc::WHITE);
  fill_circle(196, 216, 8, key_action_to_bool(key_events.at(KeyAction::RotateRight)), olc::GREEN,
              olc::BLACK);
  fill_circle(242, 216, 8, key_action_to_bool(key_events.at(KeyAction::RotateLeft)), olc::GREEN,
              olc::BLACK);
}

void Renderer::renderBackground(const std::string background_sprite) {
  if (background_rendered_ != background_sprite) {
    render_engine_ref_.DrawSprite(0, 0, getSprite(background_sprite));
    background_rendered_ = background_sprite;
  }
}

// This is a little hacky as it undermines the renderBackground function.
// TODO:: Send line clear animation info to the renderGameState fn.
void Renderer::doTetrisFlash(const int &line_clear_frame_number) {
  const auto &frame = line_clear_frame_number;
  if ((frame - 1) % 4 == 0) {
    render_engine_ref_.DrawSprite(0, 0, getSprite("background_flash"));
  } else {
    render_engine_ref_.DrawSprite(0, 0, getSprite("background"));
  }
}

void Renderer::renderPaused() {
  constexpr Coords paused_pos{110, 116};
  render_engine_ref_.DrawString(paused_pos.x, paused_pos.y, "PAUSED", olc::WHITE);
}

void Renderer::renderGameState(const GameState<> &state, const bool debug_mode,
                               const KeyEvents &key_events) {
  constexpr Coords grid_top_left{96, 40};
  constexpr Rect grid_size{80, 160};
  constexpr Coords counter_sprite_pos{13, 61};

  auto get_grid_for_render = [](const GameState<> &state) {
    if (entryDelay(state)) {
      return state.grid;
    } else {
      return addTetrominoToGrid(state.grid, state.active_tetromino);
    }
  };
  renderBackground("background");
  // Clear the field.
  render_engine_ref_.FillRect(grid_top_left.x, grid_top_left.y, grid_size.width, grid_size.height,
                              olc::BLACK);

  render_engine_ref_.DrawSprite(counter_sprite_pos.x, counter_sprite_pos.y,
                                getSprite("l" + std::to_string(state.level % 10) + "-counts"));
  renderGrid(grid_top_left.x, grid_top_left.y, get_grid_for_render(state), state.level);
  renderNextTetromino(state.next_tetromino, state.level);
  renderText(state);
  if (debug_mode) {
    renderDebug(state, key_events);
  }
  if (state.paused) {
    renderPaused();
  }
}

void Renderer::renderMenu(const MenuState &menu_state) {
  renderBackground("a-type-background");

  renderLevelSelector(menu_state.level);
}

void Renderer::renderLevelSelector(const int level) {
  constexpr Coords level_selector{51, 75};
  constexpr Rect level_selector_size{83, 35};
  constexpr Rect selector_size{16, 16};
  const olc::Pixel selector_color(252, 151, 56);

  // Clear the selector.
  render_engine_ref_.FillRect(level_selector.x, level_selector.y, level_selector_size.width,
                              level_selector_size.height, olc::BLACK);

  auto get_selector_coords = [&](const int level) -> Coords {
    const int y = (level % 10 > 4) ? level_selector.y + selector_size.height : level_selector.y;
    return {(selector_size.width * (level % 5)) + level_selector.x + 1, y + 2};
  };

  // Draw the flashing selector, and then draw the levels on top with transparency.
  const auto coords = get_selector_coords(level);
  const auto color = frame_counter_ % 4 ? selector_color : olc::BLACK;
  render_engine_ref_.FillRect(coords.x, coords.y, selector_size.width, selector_size.height, color);
  render_engine_ref_.DrawSprite(level_selector.x, level_selector.y, getSprite("levels"));

  ++frame_counter_;
}
};  // namespace tetris_clone
