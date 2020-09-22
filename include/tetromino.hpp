#pragma once

#include <tuple>
#include <vector>

namespace tetris_clone {

enum class Tetromino { T, J, Z, Square, S, L, Line };

struct TetrominoState {
  Tetromino tetromino;
  int x;
  int y;
  int rotation;
};

using TetrominoGrid = std::vector<std::vector<int>>;

// Tetrominos defines below. The matrices are column-major, resulting in access as matrix[x][y].
// The default rotation for nes tetris is defined first, followed by clockwise rotation order. This
// results in moving +1 through the vector results in a clockwise rotation, and likewise -1 results
// in a anticlockwise rotation.

const std::vector<TetrominoGrid> T_TETROMINO{{{{0, 1, 0}, {0, 1, 1}, {0, 1, 0}}},
                                             {{{0, 1, 0}, {1, 1, 1}, {0, 0, 0}}},
                                             {{{0, 1, 0}, {1, 1, 0}, {0, 1, 0}}},
                                             {{{0, 0, 0}, {1, 1, 1}, {0, 1, 0}}}};

const std::vector<TetrominoGrid> J_TETROMINO{{{{0, 1, 0}, {0, 1, 0}, {0, 1, 1}}},
                                             {{{0, 0, 1}, {1, 1, 1}, {0, 0, 0}}},
                                             {{{1, 1, 0}, {0, 1, 0}, {0, 1, 0}}},
                                             {{{0, 0, 0}, {1, 1, 1}, {1, 0, 0}}}};

const std::vector<TetrominoGrid> L_TETROMINO{{{{0, 1, 1}, {0, 1, 0}, {0, 1, 0}}},
                                             {{{1, 0, 0}, {1, 1, 1}, {0, 0, 0}}},
                                             {{{0, 1, 0}, {0, 1, 0}, {1, 1, 0}}},
                                             {{{0, 0, 0}, {1, 1, 1}, {0, 0, 1}}}};

const std::vector<TetrominoGrid> S_TETROMINO{{{{0, 0, 1}, {0, 1, 1}, {0, 1, 0}}},
                                             {{{0, 0, 0}, {1, 1, 0}, {0, 1, 1}}}};

const std::vector<TetrominoGrid> Z_TETROMINO{{{{0, 1, 0}, {0, 1, 1}, {0, 0, 1}}},
                                             {{{0, 0, 0}, {0, 1, 1}, {1, 1, 0}}}};

const std::vector<TetrominoGrid> I_TETROMINO{
    {{{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}}},
    {{{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}}}};

const TetrominoGrid SQUARE_TETROMINO{{{1, 1}, {1, 1}}};

inline TetrominoGrid getTetrominoGrid(const Tetromino &tetromino, const int rotation) {
  if (tetromino == Tetromino::T) {
    return T_TETROMINO[rotation];
  } else if (tetromino == Tetromino::J) {
    return J_TETROMINO[rotation];
  } else if (tetromino == Tetromino::Z) {
    return Z_TETROMINO[rotation % 2];
  } else if (tetromino == Tetromino::S) {
    return S_TETROMINO[rotation % 2];
  } else if (tetromino == Tetromino::L) {
    return L_TETROMINO[rotation];
  } else if (tetromino == Tetromino::Line) {
    return I_TETROMINO[rotation % 2];
  } else {
    return SQUARE_TETROMINO;
  }
}

inline TetrominoGrid getTetrominoGrid(const TetrominoState &tetromino) {
  return getTetrominoGrid(tetromino.tetromino, tetromino.rotation);
}

inline int getColor(const Tetromino &tetromino) {
  if (tetromino == Tetromino::T || tetromino == Tetromino::Square || tetromino == Tetromino::Line) {
    return 1;
  } else if (tetromino == Tetromino::J || tetromino == Tetromino::S) {
    return 2;
  } else {
    return 3;
  }
}

inline std::tuple<int, int> getStartOffsets(const Tetromino &tetromino) {
  if (tetromino == Tetromino::T || tetromino == Tetromino::J || tetromino == Tetromino::L ||
      tetromino == Tetromino::S || tetromino == Tetromino::Z) {
    return {1, 1};
  } else if (tetromino == Tetromino::Line) {
    return {2, 2};
  } else {
    return {1, 0};
  }
}

}  // namespace tetris_clone
