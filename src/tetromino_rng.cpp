#include "tetromino_rng.hpp"

#include <algorithm>
#include <map>

#include "tetromino.hpp"

namespace nestris_x86 {

UniformTetrominoRNG::UniformTetrominoRNG() : random_number_engine_{}, random_generator_{0, 6} {}

Tetromino UniformTetrominoRNG::getRandomTetromino() {
  return Tetromino{random_generator_(random_number_engine_)};
}

// Taken from https://meatfighter.com/nintendotetrisai/#Spawning_Tetriminos
const std::map<Tetromino, int> BIASED_TETROMINO_COUNTS{{Tetromino::T, 9},       //
                                                       {Tetromino::J, 8},       //
                                                       {Tetromino::Z, 8},       //
                                                       {Tetromino::Square, 8},  //
                                                       {Tetromino::S, 9},       //
                                                       {Tetromino::L, 7},       //
                                                       {Tetromino::Line, 7}};   //

std::vector<Tetromino> createBiasedLookupTable(const std::map<Tetromino, int>& tetromino_counts) {
  std::vector<Tetromino> biased_lookup;
  int idx = 0;
  for (const auto& [tetromino, count] : tetromino_counts) {
    for (int i = 0; i < count; ++i) {
      biased_lookup.push_back(tetromino);
    }
  }
  return biased_lookup;
}

NesTetrominoRNG::NesTetrominoRNG()
    : random_number_engine_{},
      random_generator_8_{0, 7},
      random_generator_56_{0, 55},
      biased_lookup_{createBiasedLookupTable(BIASED_TETROMINO_COUNTS)},
      last_tetromino_{} {}

Tetromino NesTetrominoRNG::getRandomTetromino() {
  // The following is a rough approximation of how the random tetromino generation code worked
  // in the original nes implementation. A detailed description of the code may be found here:
  // https://meatfighter.com/nintendotetrisai/#Spawning_Tetriminos
  const auto roll_1 = random_generator_8_(random_number_engine_);
  Tetromino ret_val{};
  if (roll_1 == 7 || Tetromino{roll_1} == last_tetromino_) {
    ret_val = biasedReroll();
  } else {
    ret_val = Tetromino{roll_1};
  }
  last_tetromino_ = ret_val;
  return ret_val;
}

Tetromino NesTetrominoRNG::biasedReroll() {
  const auto reroll = random_generator_56_(random_number_engine_);
  return biased_lookup_.at(reroll);
}

SevenBagTetrominoRNG::SevenBagTetrominoRNG() : random_number_engine_{}, seven_bag_{}, idx_{0} {
  for (int i = 0; i < 7; ++i) {
    seven_bag_[i] = Tetromino{i};
  }
  shuffleBag();
}

void SevenBagTetrominoRNG::shuffleBag() {
  std::shuffle(seven_bag_.begin(), seven_bag_.end(), random_number_engine_);
}

Tetromino SevenBagTetrominoRNG::getRandomTetromino() {
  if (idx_ > 6) {
    shuffleBag();
    idx_ = 0;
  }
  return seven_bag_.at(idx_++);
}

}  // namespace nestris_x86
