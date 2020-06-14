#include "tetris.hpp"

#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>

#define LOG_ERROR(msg) std::cerr << msg << std::endl;
#define LOG_INFO(msg) std::cout << msg << std::endl;

namespace tetris_clone {

void TetrisClone::RenderNextPiece(const Piece& next_piece, const int level) {
  // Clear existing piece.
  FillRect(191, 112, 33, 15, olc::BLACK);

  constexpr int three_by_two_start_x = 196;
  constexpr int three_by_two_start_y = 112;
  constexpr int square_start_x = 200;
  constexpr int square_start_y = 112;
  constexpr int line_start_x = 192;
  constexpr int line_start_y = 116;
  using VecGrid = std::vector<std::vector<int>>;
  // TODO:: Move these definitions elsewhere. I will need them again anyway for
  // the game code...  maybe they can be shared.
  if (next_piece == Piece::T) {
    VecGrid t_piece{{1, 0}, {1, 1}, {1, 0}};
    RenderGrid(three_by_two_start_x, three_by_two_start_y, t_piece, level);
  } else if (next_piece == Piece::J) {
    VecGrid j_piece{{2, 0}, {2, 0}, {2, 2}};
    RenderGrid(three_by_two_start_x, three_by_two_start_y, j_piece, level);
  } else if (next_piece == Piece::L) {
    VecGrid l_piece{{3, 3}, {3, 0}, {3, 0}};
    RenderGrid(three_by_two_start_x, three_by_two_start_y, l_piece, level);
  } else if (next_piece == Piece::Z) {
    VecGrid z_piece{{3, 0}, {3, 3}, {0, 3}};
    RenderGrid(three_by_two_start_x, three_by_two_start_y, z_piece, level);
  } else if (next_piece == Piece::S) {
    VecGrid s_piece{{0, 2}, {2, 2}, {2, 0}};
    RenderGrid(three_by_two_start_x, three_by_two_start_y, s_piece, level);
  } else if (next_piece == Piece::Square) {
    VecGrid square_piece{{1, 1}, {1, 1}};
    RenderGrid(square_start_x, square_start_y, square_piece, level);
  } else if (next_piece == Piece::Line) {
    VecGrid line_piece{{1}, {1}, {1}, {1}};
    RenderGrid(line_start_x, line_start_y, line_piece, level);
  }
}

void TetrisClone::DrawNumber(const int x, const int y, const int num,
                             const int pad, const olc::Pixel color) {
  std::stringstream ss;
  ss << std::setw(pad) << std::setfill('0') << num;
  DrawString(x, y, ss.str(), color);
}

void TetrisClone::RenderText(const GameState<>& state) {
  DrawNumber(152, 16, state.lines, 3);
  DrawNumber(192, 32, state.high_score, 6);
  DrawNumber(192, 56, state.score, 6);
  DrawNumber(208, 160, state.level, 2);
  for (int i = 0; i < 7; ++i) {
    DrawNumber(48, 88 + (i * 16), state.piece_counts[i], 3, olc::RED);
  }
}

void TetrisClone::RenderGameState(const GameState<>& state) {
  RenderGrid(96, 40, state.grid, state.level);
  RenderNextPiece(state.next_piece, state.level);
  RenderText(state);
}

bool TetrisClone::OnUserCreate() {
  if (ScreenWidth() != 256 || ScreenHeight() != 225) {
    LOG_ERROR("Screen size must be set to 256x240 for this application.");
    return false;
  }
  bg_ptr_ = std::make_unique<olc::Sprite>("assets/basic_field_01.png");
  block_sprites_.resize(10);
  block_sprites_[8].emplace_back(std::make_unique<olc::Sprite>(8, 8));
  block_sprites_[8].emplace_back(
      std::make_unique<olc::Sprite>("assets/l8-c0.png"));
  block_sprites_[8].emplace_back(
      std::make_unique<olc::Sprite>("assets/l8-c1.png"));
  block_sprites_[8].emplace_back(
      std::make_unique<olc::Sprite>("assets/l8-c2.png"));

  DrawSprite(0, 0, bg_ptr_.get());
  return true;
}

bool TetrisClone::OnUserUpdate(float fElapsedTime) {
  GameState<> state;
  state.level = 8;
  state.grid[0][19] = 2;
  state.grid[1][19] = 2;
  state.grid[3][19] = 3;
  state.grid[4][19] = 3;
  state.grid[9][19] = 1;
  state.grid[2][18] = 2;
  state.grid[3][18] = 2;
  state.grid[4][18] = 3;
  state.grid[5][18] = 3;
  state.grid[8][18] = 1;
  RenderGameState(state);
  return true;
}

}  // namespace tetris_clone
