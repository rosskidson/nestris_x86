#include "game_logic.hpp"

#include "game_states.hpp"
#include "gravity.hpp"

namespace nestris_x86 {

constexpr int LINES_PER_LEVEL = 10;
const std::vector<int> line_scores{0, 40, 100, 300, 1200};

int getScoreForLineClear(const int lines_cleared, const int level) {
  if (lines_cleared > 4) {
    throw std::runtime_error("Too many simultaneous lines cleared to get a score.");
  }
  return level * line_scores[lines_cleared];
}

GameState<>::Grid addTetrominoToGrid(const GameState<>::Grid &grid,
                                     const TetrominoState &tetromino) {
  GameState<>::Grid grid_copy(grid);
  const auto &tetromino_grid = getTetrominoGrid(tetromino);
  const auto [x_offset, y_offset] = getStartOffsets(tetromino.tetromino);
  const int x_start = tetromino.x - x_offset;
  const int y_start = tetromino.y - y_offset;
  const int color = getColor(tetromino.tetromino);
  for (int i = 0; i < tetromino_grid.size(); ++i) {
    for (int j = 0; j < tetromino_grid[i].size(); ++j) {
      if (outOfBounds(grid_copy, x_start + i, y_start + j) || not tetromino_grid[i][j]) {
        continue;
      }
      grid_copy[x_start + i][y_start + j] = tetromino_grid[i][j] * color;
    }
  }
  return grid_copy;
}

bool tetrominoCollision(const GameState<>::Grid &grid, const TetrominoState &tetromino) {
  const auto &tetromino_grid = getTetrominoGrid(tetromino);
  const auto [x_offset, y_offset] = getStartOffsets(tetromino.tetromino);
  const int x_start = tetromino.x - x_offset;
  const int y_start = tetromino.y - y_offset;
  for (int i = 0; i < tetromino_grid.size(); ++i) {
    for (int j = 0; j < tetromino_grid[i].size(); ++j) {
      if (not tetromino_grid[i][j]) {
        continue;
      }
      // Special case: If out of bounds above the play field, this is not a
      // collision (providing it is still within x bounds).
      if ((y_start + j < 0) && (x_start + i >= 0) && (x_start + i < grid.size())) {
        continue;
      }
      if (outOfBounds(grid, x_start + i, y_start + j) || grid[x_start + i][y_start + j]) {
        return true;
      }
    }
  }
  return false;
}

bool updateStateOnNoCollision(const GameState<>::Grid &grid, const int tetromino_x_offset,
                              const int tetromino_y_offset, const int tetromino_rotation_offset,
                              TetrominoState &tetromino) {
  TetrominoState test_tetromino{tetromino};
  test_tetromino.x += tetromino_x_offset;
  test_tetromino.y += tetromino_y_offset;
  auto &r = test_tetromino.rotation;
  r += tetromino_rotation_offset;
  r = r < 0 ? 3 : r;
  r = r > 3 ? 0 : r;
  if (not tetrominoCollision(grid, test_tetromino)) {
    tetromino = test_tetromino;
    return true;
  } else {
    return false;
  }
}

bool attemptWallKickRotate(const GameState<>::Grid &grid, const int tetromino_rotation_offset,
                           TetrominoState &tetromino) {
  if (updateStateOnNoCollision(grid, 0, 0, tetromino_rotation_offset, tetromino)) {
    return true;
  }
  for (int i = 1; i <= 2; ++i) {
    if (updateStateOnNoCollision(grid, i, 0, tetromino_rotation_offset, tetromino) ||
        updateStateOnNoCollision(grid, -i, 0, tetromino_rotation_offset, tetromino)) {
      return true;
    }
  }
  return false;
}

bool rotateTetromino(const GameState<>::Grid &grid, const int rotation, const bool wall_kick,
                     TetrominoState &tetromino) {
  if (wall_kick) {
    return attemptWallKickRotate(grid, rotation, tetromino);
  } else {
    return updateStateOnNoCollision(grid, 0, 0, rotation, tetromino);
  }
}

void hardDrop(const GameState<>::Grid &grid, TetrominoState &tetromino) {
  while (updateStateOnNoCollision(grid, 0, 1, 0, tetromino))
    ;
}

void processKeyEvents(const KeyEvents &key_events, const sound::SoundPlayer &sample_player,
                      const Das &das_processor, const bool wall_kick, const bool hard_drop_enable,
                      GameState<> &state) {
  auto move_check_wall_charge = [&sample_player, &das_processor](GameState<> &state,
                                                                 const int direction) {
    if (updateStateOnNoCollision(state.grid, direction, 0, 0, state.active_tetromino)) {
      sample_player.playSample("tetromino_move");
    } else {
      das_processor.fullyChargeDas(state.das_counter);
      state.viz_wall_charge_frame_count = 30;
    }
  };

  if (key_events.at(KeyAction::Up).pressed && hard_drop_enable) {
    hardDrop(state.grid, state.active_tetromino);
    state.gravity_counter = 0;
  }

  if (key_events.at(KeyAction::Left).pressed) {
    das_processor.hardResetDas(state.das_counter);
    move_check_wall_charge(state, -1);
  }
  if (key_events.at(KeyAction::Right).pressed) {
    das_processor.hardResetDas(state.das_counter);
    move_check_wall_charge(state, +1);
  }
  auto das_trigger = [&das_processor](int &das_counter) {
    ++das_counter;
    if (das_processor.dasFullyCharged(das_counter)) {
      das_processor.softResetDas(das_counter);
      return true;
    }
    return false;
  };
  if (key_events.at(KeyAction::Left).held) {
    if (das_trigger(state.das_counter)) {
      move_check_wall_charge(state, -1);
    }
  }
  if (key_events.at(KeyAction::Right).held) {
    if (das_trigger(state.das_counter)) {
      move_check_wall_charge(state, +1);
    }
  }

  if (key_events.at(KeyAction::Down).held) {
    auto &gr = state.gravity_counter;
    gr = (gr > 2) && not state.press_down_lock ? 2 : gr;
  } else {
    state.press_down_lock = false;
    state.press_down_counter = 0;
  }

  int rotation = 0;
  rotation = key_events.at(KeyAction::RotateClockwise).pressed ? 1 : rotation;
  rotation = key_events.at(KeyAction::RotateAntiClockwise).pressed ? -1 : rotation;
  if (rotation != 0 && rotateTetromino(state.grid, rotation, wall_kick, state.active_tetromino)) {
    sample_player.playSample("tetromino_rotate");
  }

  if (key_events.at(KeyAction::Start).pressed) {
    state.paused = true;
    sample_player.playSample("pause");
  }
}

void clearLine(const int row, GameState<> &state) {
  for (int y = row; y > 0; --y) {
    for (int x = 0; x < state.grid.size(); ++x) {
      state.grid[x][y] = state.grid[x][y - 1];
    }
  }
  for (int x = 0; x < state.grid.size(); ++x) {
    state.grid[x][0] = 0;
  }
}

std::vector<int> checkForLineClears(const GameState<> &state) {
  std::vector<int> complete_lines{};
  auto is_line_complete = [](const GameState<>::Grid &grid, const int row) {
    for (int x = 0; x < grid.size(); ++x) {
      if (grid[x][row] == 0) {
        return false;
      }
    }
    return true;
  };
  for (int y = 0; y < state.grid[0].size(); ++y) {
    if (is_line_complete(state.grid, y)) {
      complete_lines.push_back(y);
    }
  }
  return complete_lines;
}

void updateEntryDelayForLineClear(int &delay_counter) {
  // Emulate a quirk in the nes implementation: The animation would only
  // start on certain frames, randomly increasing/decreasing the ARE.
  // TODO:: -use modern c++ random and hold the random seed in class state
  delay_counter += (17 + (rand() % 5));
}

void updateScoreAndLevel(const int line_clears, const sound::SoundPlayer &sound_player,
                         GameState<> &state) {
  state.score += getScoreForLineClear(line_clears, state.level);
  state.lines += line_clears;
  state.lines_until_next_level -= line_clears;
  if (state.lines_until_next_level <= 0) {
    ++state.level;
    sound_player.playSample("level_up");
    state.lines_until_next_level += LINES_PER_LEVEL;
  }
}

int getEntryDelayFromLockHeight(const int height) {
  return height * -0.5 + 19;
}

bool applyGravity(const KeyEvents &key_events, const Gravity &gravity_provider,
                  GameState<> &state) {
  if (--state.gravity_counter <= 0) {
    // Apply press down scoring.
    if (key_events.at(KeyAction::Down).held) {
      auto &down = state.press_down_counter;
      down = ++down > 15 ? 10 : down;
    }
    state.gravity_counter = gravity_provider.getGravity(state.level);
    if (not updateStateOnNoCollision(state.grid, 0, 1, 0, state.active_tetromino)) {
      state.entry_delay_counter = getEntryDelayFromLockHeight(state.active_tetromino.y);
      state.spawn_new_tetromino = true;
      state.grid = addTetrominoToGrid(state.grid, state.active_tetromino);
      state.active_tetromino.y = -10;
      return true;
    }
  }
  return false;
}

bool updateTopOutState(const KeyEvents &key_events, int &top_out_frame_counter,
                       GameState<> &state) {
  top_out_frame_counter++;
  constexpr int start_frame = 60;
  constexpr int animation_step = 4;
  constexpr int end_frame = start_frame + (20 * animation_step);
  if (key_events.at(KeyAction::Start).pressed) {
    if (top_out_frame_counter >= end_frame) {
      return true;
    } else {
      top_out_frame_counter = end_frame;
    }
  }
  if (top_out_frame_counter < start_frame || top_out_frame_counter > end_frame ||
      (top_out_frame_counter - start_frame) % animation_step) {
    return false;
  }
  const int top_out_step = (top_out_frame_counter - start_frame) / animation_step;
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < top_out_step; ++j) {
      state.grid[i][j] = 4;
    }
  }
  return false;
}

int linesToClearFromStartingLevel(const int level) {
  if (level < 10) {
    return level * 10 + 10;
  } else {
    return std::max(100, level * 10 - 50);
  }
}

void animateLineClear(const sound::SoundPlayer &sample_player, GameState<> &state,
                      LineClearAnimationInfo &line_clear_info) {
  auto &frame = line_clear_info.animation_frame;
  if (frame == 0) {
    return;
  }
  --frame;

  if (frame == 23) {
    if (line_clear_info.rows.size() == 4) {
      sample_player.playSample("tetris");
    } else if (line_clear_info.rows.size() > 0) {
      sample_player.playSample("line_clear");
    }
  } else if (frame > 21) {
    // Still waiting for entry delay to end.
    return;
  } else if (frame >= 5) {
    // Line clear animation.
    const int blocks_to_remove = 6 - ((frame - 1) / 4);
    for (const auto &row : line_clear_info.rows) {
      for (int i = 4; i > 4 - blocks_to_remove; --i) {
        state.grid[i][row] = 0;
        state.grid[9 - i][row] = 0;
      }
    }
  } else if (frame == 3) {
    // Move lines down.
    for (const auto &row : line_clear_info.rows) {
      clearLine(row, state);
    }
  }
}

void addPressDownScore(GameState<> &state) {
  state.score += state.press_down_counter;
  state.press_down_counter = 0;
}

}  // namespace nestris_x86
