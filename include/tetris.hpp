#pragma once

#include <memory>
#include <vector>

#include "olcPixelGameEngine.h"

namespace tetris_clone {

enum class Piece { T, J, Z, Square, S, L, Line };

template <int W = 10, int H = 20>
struct GameState {
  GameState()
      : grid{},
        level{},
        lines{},
        next_piece{Piece::Z},
        score{},
        high_score{},
        piece_counts{} {}

  using Grid = std::array<std::array<int, H>, W>;
  Grid grid;
  int level;
  int lines;
  Piece next_piece;
  int score;
  int high_score;
  std::array<int, 7> piece_counts;  // Ordered according to enum.
};

class TetrisClone : public olc::PixelGameEngine {
 public:
  TetrisClone() { sAppName = "TetrisClone"; }

 public:
  bool OnUserCreate() override;

  bool OnUserUpdate(float fElapsedTime) override;

 private:
  void RenderGameState(const GameState<>& state);
  void RenderNextPiece(const Piece& next_piece, const int level);
  void DrawNumber(const int x, const int y, const int num, const int pad,
                  const olc::Pixel color = olc::WHITE);
  void RenderText(const GameState<>& state);

  template <typename GridContainer>
  void RenderGrid(const int x_start, const int y_start,
                  const GridContainer& grid, const int level,
                  const int x_spacing = 8, const int y_spacing = 8) {
    for (int i = 0; i < grid.size(); ++i) {
      for (int j = 0; j < grid[i].size(); ++j) {
        DrawSprite(x_start + i * x_spacing, y_start + j * y_spacing,
                   block_sprites_[level][grid[i][j]].get());
      }
    }
  }

  std::unique_ptr<olc::Sprite> bg_ptr_;
  std::vector<std::vector<std::unique_ptr<olc::Sprite>>> block_sprites_;
};

}  // namespace tetris_clone
