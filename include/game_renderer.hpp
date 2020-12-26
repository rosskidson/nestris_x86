#pragma once

#include <memory>
#include <set>

#include "assets.hpp"
#include "das.hpp"
#include "drawers/pixel_drawing_interface.hpp"
#include "game_states.hpp"
#include "key_defines.hpp"
#include "tetromino.hpp"

namespace tetris_clone {
class GameRenderer {
 public:
  GameRenderer(std::unique_ptr<PixelDrawingInterface> &&drawer,
               const std::shared_ptr<SpriteProvider> &sprite_provider,
               const std::string &sprites_path);

  void renderGameState(const GameState<> &state, const bool render_controls,
                       const bool render_das_bar, const bool render_entry_delay,
                       const KeyEvents &key_events, const Das &das_processor);

  void renderPaused() const;

  void doTetrisFlash(const int &line_clear_frame_number) const;

  void startNewGame();

 private:
  template <typename GridContainer>
  void renderGrid(const int x_start, const int y_start, const GridContainer &grid, const int level,
                  const int x_spacing = 8, const int y_spacing = 8, const int color = 1) const {
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

  olc::Sprite *getBlockSprite(const int level, const int color) const;

  void renderBackground();
  void renderText(const GameState<> &state) const;
  void renderNextTetromino(const Tetromino &next_tetromino, const int level) const;
  void renderDasBar(const int das_counter, const Das &das_processor,
                    const PixelDrawingInterface::Coords &das_box_pos) const;
  void renderControls(const GameState<> &state, const KeyEvents &key_events,
                      const PixelDrawingInterface::Coords &control_position) const;
  void renderEntryDelay(const bool delay_entry,
                        const PixelDrawingInterface::Coords &position) const;

  std::unique_ptr<PixelDrawingInterface> drawer_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  std::vector<std::vector<std::unique_ptr<olc::Sprite>>> block_sprites_;
  bool background_rendered_;
};

}  // namespace tetris_clone
