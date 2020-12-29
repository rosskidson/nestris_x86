#pragma once

#include <array>
#include <map>
#include <memory>

#include "key_defines.hpp"
#include "tetromino.hpp"

namespace nestris_x86 {

template <int W = 10, int H = 20>
// clang-format off
struct GameState {
  GameState()
      : grid{},
        das_counter{},
        gravity_counter{},
        entry_delay_counter{},
        spawn_new_tetromino{},
        level{},
        lines{},
        lines_until_next_level{},
        active_tetromino{},
        next_tetromino{},
        score{},
        high_score{},
        //tetromino_counts{},
        topped_out{},
        paused{} {}
  // clang-format on

  using Grid = std::array<std::array<int, H>, W>;
  Grid grid;
  int gravity_counter;
  int das_counter;
  int entry_delay_counter;
  bool spawn_new_tetromino;
  int level;
  int lines;
  int lines_until_next_level;
  TetrominoState active_tetromino;
  Tetromino next_tetromino;
  int score;
  int high_score;
  //std::array<int, 7> tetromino_counts;  // Ordered according to enum.
  bool topped_out;
  bool paused;
  bool press_down_lock;
  int press_down_counter;
};

inline bool entryDelay(const GameState<>& state) {
  return state.entry_delay_counter > 0;
}

struct LineClearAnimationInfo {
  std::vector<int> rows;
  int animation_frame{};
};

}  // namespace nestris_x86
