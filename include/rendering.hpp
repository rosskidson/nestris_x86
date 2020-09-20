#pragma once

#include "game_state.hpp"
#include "key_defines.hpp"
#include "olcPixelGameEngine.h"
#include "tetromino.hpp"

namespace tetris_clone {
class Renderer {
public:
  Renderer(olc::PixelGameEngine &render_engine, const std::string &sprites_path);

  void init();

  void renderGameState(const GameState<> &state, const bool debug_mode,
                       const KeyStates &key_states);

  void renderPaused();

  void doTetrisFlash(const int &line_clear_frame_number);

private:

  inline olc::Sprite *getBlockSprite(const int level, const int color) {
    return block_sprites_.at(level % 10).at(color).get();
  }

  inline olc::Sprite *getSprite(const std::string &sprite_name) {
    return sprite_map_.at(sprite_name).get();
  }

  template <typename GridContainer>
  void renderGrid(const int x_start, const int y_start, const GridContainer &grid, const int level,
                  const int x_spacing = 8, const int y_spacing = 8, const int color = 1) {
    for (int i = 0; i < grid.size(); ++i) {
      for (int j = 0; j < grid[i].size(); ++j) {
        if (grid[i][j] == 0) {
          continue;
        }
        render_engine_ref_.DrawSprite(x_start + i * x_spacing, y_start + j * y_spacing,
                                      getBlockSprite(level, grid[i][j] * color));
      }
    }
  }

  void drawNumber(const int x, const int y, const int num, const int pad,
                  const olc::Pixel color = olc::WHITE);

  void renderText(const GameState<> &state);

  void renderNextTetromino(const Tetromino &next_tetromino, const int level);

  void renderDebug(const GameState<> &state, const KeyStates &key_states);

  std::map<std::string, std::unique_ptr<olc::Sprite>> sprite_map_;
  std::vector<std::vector<std::unique_ptr<olc::Sprite>>> block_sprites_;
  olc::PixelGameEngine &render_engine_ref_;
};

} // namespace tetris_clone
