#pragma once

#include <memory>
#include <set>

#include "assets.hpp"
#include "das.hpp"
#include "game_states.hpp"
#include "key_defines.hpp"
#include "pixel_drawing_interface.hpp"
#include "tetromino.hpp"
#include "utils/logging.hpp"

namespace tetris_clone {
class Renderer {
 public:
  Renderer(std::unique_ptr<PixelDrawingInterface> &&drawer,
           const std::shared_ptr<SpriteProvider> &sprite_provider,
           const std::string &sprites_path);

  void renderGameState(const GameState<> &state, const bool render_controls,
                       const bool render_das_bar, const bool render_entry_delay,
                       const KeyEvents &key_events, const Das &das_processor);

  void renderPaused();

  void doTetrisFlash(const int &line_clear_frame_number);

 private:
  inline olc::Sprite *getBlockSprite(const int level, const int color) try {
    return block_sprites_.at(level % 10).at(color).get();
  } catch (const std::out_of_range &e) {
    LOG_ERROR("Block sprite not loaded. Level[" << level << "] Color[" << color << "]");
    throw;
  }

  template <typename GridContainer>
  void renderGrid(const int x_start, const int y_start, const GridContainer &grid, const int level,
                  const int x_spacing = 8, const int y_spacing = 8, const int color = 1) {
    for (int i = 0; i < grid.size(); ++i) {
      for (int j = 0; j < grid[i].size(); ++j) {
        if (grid[i][j] == 0) {
          continue;
        }
        drawer_->drawSprite(x_start + i * x_spacing, y_start + j * y_spacing,
                            getBlockSprite(level, grid[i][j] * color));
      }
    }
  }

  // #### BASIC/COMMON ######
  void renderBackground(const std::string background_sprite);

  // #### GAME SCREEN ######
  void renderText(const GameState<> &state);
  void renderNextTetromino(const Tetromino &next_tetromino, const int level);
  void renderDasBar(const int das_counter, const Das &das_processor, const int x, const int y);
  void renderControls(const GameState<> &state, const KeyEvents &key_events, const int x,
                      const int y);
  void renderEntryDelay(const bool delay_entry, const int x, const int y);

  std::string background_rendered_;
  std::unique_ptr<PixelDrawingInterface> drawer_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  std::map<std::string, std::unique_ptr<olc::Sprite>> sprite_map_;
  std::vector<std::vector<std::unique_ptr<olc::Sprite>>> block_sprites_;
  // olc::PixelGameEngine &render_engine_ref_;
  int frame_counter_;
};  // namespace tetris_clone

}  // namespace tetris_clone
