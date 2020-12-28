#pragma once

#include <vector>

#include "tetris_type.hpp"

namespace nestris_x86 {

const std::vector<int> NTSC_GRAVITY{48, 43, 38, 33, 28, 23, 18, 13, 8, 6, 5, 5, 5, 4, 4,
                                    4,  3,  3,  3,  2,  2,  2,  2,  2, 2, 2, 2, 2, 2};

const std::vector<int> PAL_GRAVITY{36, 32, 29, 25, 22, 18, 15, 11, 7, 5,
                                   4,  4,  4,  3,  3,  3,  2,  2,  2, 1};

class Gravity {
 public:
  Gravity(const std::vector<int>& level_gravity_values)
      : level_gravity_values_(level_gravity_values) {}

  Gravity(const TetrisType& type)
      : Gravity(type == TetrisType::NTSC ? NTSC_GRAVITY : PAL_GRAVITY) {}

  inline int getGravity(const int level) const {
    if (level >= level_gravity_values_.size()) {
      return 1;
    } else {
      return level_gravity_values_.at(level);
    }
  }

 private:
  std::vector<int> level_gravity_values_;
};

}  // namespace nestris_x86
