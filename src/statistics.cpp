#include "statistics.hpp"

#include "game_logic.hpp"

namespace nestris_x86 {

Statistics::Statistics()
    : tetromino_counts_{
          {Tetromino::T, 0},       //
          {Tetromino::J, 0},       //
          {Tetromino::Z, 0},       //
          {Tetromino::Square, 0},  //
          {Tetromino::S, 0},       //
          {Tetromino::L, 0},       //
          {Tetromino::Line, 0}     //
      },
  score_from_tetrises_{},
  long_bar_drought_{},
  das_chain_counter_{},
  burn_counter_{}
{}

void Statistics::update(const int lines_cleared, const int level) {
  if (lines_cleared == 4) {
    score_from_tetrises_ += getScoreForLineClear(lines_cleared, level);
    burn_counter_ = 0;
  } else {
    burn_counter_ += lines_cleared;
  }
}

void Statistics::update(const Tetromino& new_tetromino) {
  tetromino_counts_[new_tetromino]++;
  long_bar_drought_ = (new_tetromino == Tetromino::Line) ? 0 : long_bar_drought_ + 1;
  das_chain_counter_++;
}

void Statistics::dasResetSignal() {
  das_chain_counter_ = 0;
}

int Statistics::getTetrominoCount(const Tetromino& tetromino) const {
  return tetromino_counts_.at(tetromino);
}

double Statistics::getTetrisRate(const int current_score) const {
  return (double)score_from_tetrises_ / std::max(1, current_score);
}

int Statistics::getBurnCount() const {
  return burn_counter_;
}

int Statistics::getLongBarDrought() const {
  return long_bar_drought_;
}

int Statistics::getDasChain() const {
  return das_chain_counter_;
}

}  // namespace nestris_x86

