#include "rendering.hpp"

#include "game_logic.hpp"
#include "assets.hpp"
#include "olcPixelGameEngine.h"

namespace tetris_clone {

Renderer::Renderer(olc::PixelGameEngine &render_engine, const std::string &sprites_path)
    : render_engine_ref_(render_engine) {
  if (not loadBlockSprites(sprites_path, block_sprites_)) {
    throw std::runtime_error("Failed loading block sprites.");
  }
  if (not loadSprites(sprites_path, sprite_map_)) {
    throw std::runtime_error("Failed loading sprites.");
  }
}

void Renderer::drawNumber(const int x, const int y, const int num, const int pad,
                          const olc::Pixel color) {
  render_engine_ref_.FillRect(x, y, 8 * pad, 8, olc::BLACK);
  std::stringstream ss;
  ss << std::setw(pad) << std::setfill('0') << num;
  render_engine_ref_.DrawString(x, y, ss.str(), color);
}

void Renderer::renderText(const GameState<> &state) {
  drawNumber(152, 16, state.lines, 3);
  drawNumber(192, 32, state.high_score, 6);
  drawNumber(192, 56, state.score, 6);
  drawNumber(208, 160, state.level, 2);
  for (int i = 0; i < 7; ++i) {
    drawNumber(48, 88 + (i * 16), state.tetromino_counts[i], 3, olc::RED);
  }
}

void Renderer::renderNextTetromino(const Tetromino &next_tetromino, const int level) {
  auto get_plotting_coords = [](const Tetromino &next_tetromino) -> std::tuple<int, int> {
    constexpr int start_x = 196;
    constexpr int start_y = 104;
    int x, y;
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
  const auto [x, y] = get_plotting_coords(next_tetromino);
  renderGrid(x, y, tetromino, level, 8, 8, getColor(next_tetromino));
}

void Renderer::renderDebug(const GameState<> &state, const KeyStates &key_states) {
  auto get_das_color = [](const int &das) {
    if (das >= 10) {
      return olc::GREEN;
    } else if (das >= 6) {
      return olc::YELLOW;
    } else {
      return olc::RED;
    };
  };
  const auto color = get_das_color(state.das_counter);
  render_engine_ref_.FillRect(200, 180, 3 * 16, 6, olc::BLACK);
  render_engine_ref_.FillRect(200, 180, 3 * state.das_counter, 6, color);

  auto fill_circle = [&](const int x, const int y, const int radius, const bool signal,
                         const olc::Pixel &on_color, const olc::Pixel &off_color) {
    auto color = signal ? on_color : off_color;
    render_engine_ref_.FillCircle(x, y, radius, color);
  };

  // fill_circle(194, 183, 3, state.das_reset_signal, olc::RED, olc::GREEN);

  render_engine_ref_.DrawString(190, 189, "ARE:", olc::WHITE);
  fill_circle(226, 192, 3, entryDelay(state), olc::GREEN, olc::RED);

  render_engine_ref_.DrawString(204, 199, "<  >", olc::WHITE);
  fill_circle(196, 202, 4, key_states.at(KeyAction::Left), olc::GREEN, olc::BLACK);
  fill_circle(242, 201, 4, key_states.at(KeyAction::Right), olc::GREEN, olc::BLACK);
  render_engine_ref_.DrawString(204, 207, "A  B", olc::WHITE);
  fill_circle(196, 216, 8, key_states.at(KeyAction::RotateRight), olc::GREEN, olc::BLACK);
  fill_circle(242, 216, 8, key_states.at(KeyAction::RotateLeft), olc::GREEN, olc::BLACK);
}

void Renderer::init() {
  render_engine_ref_.DrawSprite(0, 0, getSprite("background"));
}

void Renderer::doTetrisFlash(const int &line_clear_frame_number) {
  const auto &frame = line_clear_frame_number;
  if ((frame - 1) % 4 == 0) {
    render_engine_ref_.DrawSprite(0, 0, getSprite("background_flash"));
  } else {
    render_engine_ref_.DrawSprite(0, 0, getSprite("background"));
  }
}

void Renderer::renderPaused() {
  render_engine_ref_.DrawString(110, 116, "PAUSED", olc::WHITE);
}

void Renderer::renderGameState(const GameState<> &state, const bool debug_mode,
                               const KeyStates &key_states) {
  auto get_grid_for_render = [](const GameState<> &state) {
    if (entryDelay(state)) {
      return state.grid;
    } else {
      return addTetrominoToGrid(state.grid, state.active_tetromino);
    }
  };
  // Clear the field.
  render_engine_ref_.FillRect(96, 40, 80, 160, olc::BLACK);
  render_engine_ref_.DrawSprite(13, 61,
                                getSprite("l" + std::to_string(state.level % 10) + "-counts"));
  renderGrid(96, 40, get_grid_for_render(state), state.level);
  renderNextTetromino(state.next_tetromino, state.level);
  renderText(state);
  if (debug_mode) {
    renderDebug(state, key_states);
  }
}
};  // namespace tetris_clone
