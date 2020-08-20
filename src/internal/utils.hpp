#pragma once

#include <iostream>

#include "tetris.hpp"

namespace tetris_clone {

inline void dumpGridToStdOut(const GameState<>::Grid& grid) {
  std::cout << std::endl;
  for (int j = 0; j < 20; ++j) {
    for (int i = 0; i < 10; ++i) {
      std::cout << grid[i][j];
    }
    std::cout << std::endl;
  }
}

}  // namespace tetris_clone
