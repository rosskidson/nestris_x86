#pragma once

#include "tetromino.hpp"

namespace nestris_x86 {

class TetrominoRNG {
 public:
  virtual Tetromino getNextTetromino() = 0;
};

class UniformTetrominoRNG : public TetrominoRNG {
 public:
  Tetromino getNextTetromino() override;
};

class NesTetroinoRNG : public TetrominoRNG {
 public:
  Tetromino getNextTetromino() override;
};

class SevenBagTetrominoRNG : public TetrominoRNG {
 public:
  Tetromino getNextTetromino() override;
};


}  // namespace nestris_x86
