#include "tetris.hpp"
#include "game_state.hpp"

#include <vector>

namespace tetris_clone {

int getGravity(const int level);

bool entryDelay(const GameState<> &state);

template <typename Container> bool outOfBounds(const Container &container, int x, int y) {
  if (x < 0 || y < 0 || x >= container.size() || y >= container[x].size()) {
    return true;
  } else
    return false;
}

GameState<>::Grid addTetrominoToGrid(const GameState<>::Grid &grid,
                                     const TetrominoState &tetromino);

bool tetrominoCollision(const GameState<>::Grid &grid, const TetrominoState &tetromino);

bool updateStateOnNoCollision(const GameState<>::Grid &grid, const int tetromino_x_offset,
                              const int tetromino_y_offset, const int tetromino_rotation_offset,
                              TetrominoState &tetromino);

void resetDas(GameState<> &state);

void fullyChargeDas(GameState<> &state);

void processKeyEvents(const KeyEvents &key_events, const sound::SoundPlayer &sample_player,
                      GameState<> &state);

void clearLine(const int row, GameState<> &state);

std::vector<int> checkForLineClears(const GameState<> &state);

void updateEntryDelayForLineClear(int &delay_counter);

void updateScoreAndLevel(const int line_clears, const sound::SoundPlayer &sound_player,
                         GameState<> &state);

int getEntryDelayFromLockHeight(const int height);

bool applyGravity(GameState<> &state);

bool updateTopOutState(const KeyEvents &key_events, int &top_out_frame_counter, GameState<> &state);

int linesToClearFromStartingLevel(const int level);

void animateLineClear(const sound::SoundPlayer &sample_player, GameState<> &state,
                      LineClearAnimationInfo &line_clear_info);

} // namespace tetris_clone
