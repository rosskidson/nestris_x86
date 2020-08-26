#include "tetris.hpp"

#include <SDL_mixer.h> // TODO:: Light wrapper around sdl mixer

#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread> //sleep until

#include "sound.hpp"

//#define OLC_PGEX_SOUND
//#include "Extensions/olcPGEX_Sound.h"

#define LOG_ERROR(msg) std::cerr << msg << std::endl;
#define LOG_INFO(msg) std::cout << msg << std::endl;

namespace tetris_clone {

using Clock = std::chrono::high_resolution_clock;
using Duration_us = std::chrono::duration<int, std::nano>;
using Duration_ms = std::chrono::duration<int, std::milli>;
using TetrominoGrid = std::vector<std::vector<int>>;
constexpr Duration_us single_frame(16666667);
// constexpr Duration_ms single_frame(500);

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

const int GRAVITY_FIRST_FRAME = 100;
const std::vector<int> LEVEL_GRAVITY{48, 43, 38, 33, 28, 23, 18, 13, 8, 6,
                                     5,  5,  5,  4,  4,  4,  3,  3,  3};

const std::vector<int> line_scores{0, 40, 100, 300, 1200};

int getGravity(const int level) {
  if (level > 28) {
    return 1;
  } else if (level > 18) {
    return 2;
  } else {
    return LEVEL_GRAVITY.at(level);
  }
}

bool entryDelay(const GameState<> &state) { return state.entry_delay_counter > 0; }

TetrominoGrid getTetrominoGrid(const Tetromino &tetromino, const int rotation) {
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

TetrominoGrid getTetrominoGrid(const TetrominoState &tetromino) {
  return getTetrominoGrid(tetromino.tetromino, tetromino.rotation);
}

std::vector<std::vector<std::unique_ptr<olc::Sprite>>> loadSprites(const std::string &path) {
  std::vector<std::vector<std::unique_ptr<olc::Sprite>>> block_sprites;
  block_sprites.resize(10);
  for (int level = 0; level < 10; ++level) {
    block_sprites[level].emplace_back(std::make_unique<olc::Sprite>(8, 8));
    for (int color = 0; color < 3; ++color) {
      std::stringstream ss;
      ss << "/l" << level << "-c" << color << ".png";
      block_sprites[level].emplace_back(std::make_unique<olc::Sprite>(path + ss.str()));
    }
  }
  return block_sprites;
}

std::vector<std::unique_ptr<olc::Sprite>> loadCounterSprites(const std::string &path) {
  std::vector<std::unique_ptr<olc::Sprite>> counter_sprites;
  counter_sprites.resize(10);
  for (int level = 0; level < 10; ++level) {
    std::stringstream ss;
    ss << "/l" << level << "-counts.png";
    counter_sprites[level] = std::make_unique<olc::Sprite>(path + ss.str());
  }
  return counter_sprites;
}

int getColor(const Tetromino &tetromino) {
  if (tetromino == Tetromino::T || tetromino == Tetromino::Square || tetromino == Tetromino::Line) {
    return 1;
  } else if (tetromino == Tetromino::J || tetromino == Tetromino::S) {
    return 2;
  } else {
    return 3;
  }
}

std::tuple<int, int> getStartOffsets(const Tetromino &tetromino) {
  if (tetromino == Tetromino::T || tetromino == Tetromino::J || tetromino == Tetromino::L ||
      tetromino == Tetromino::S || tetromino == Tetromino::Z) {
    return {1, 1};
  } else if (tetromino == Tetromino::Line) {
    return {2, 2};
  } else {
    return {1, 0};
  }
}

template <typename Container> bool outOfBounds(const Container &container, int x, int y) {
  if (x < 0 || y < 0 || x >= container.size() || y >= container[x].size()) {
    return true;
  } else
    return false;
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

KeyBindings getKeyBindings() {
  KeyBindings key_bindings;
  key_bindings[KeyAction::Up] = olc::Key::UP;
  key_bindings[KeyAction::Down] = olc::Key::DOWN;
  key_bindings[KeyAction::Left] = olc::Key::LEFT;
  key_bindings[KeyAction::Right] = olc::Key::RIGHT;
  key_bindings[KeyAction::RotateLeft] = olc::Key::X;
  key_bindings[KeyAction::RotateRight] = olc::Key::Z;
  key_bindings[KeyAction::Start] = olc::Key::ENTER;
  return key_bindings;
}

KeyEvent getButtonState(const bool button_old_state, const bool button_new_state) {
  KeyEvent event;
  event.pressed = not button_old_state && button_new_state;
  event.released = button_old_state && not button_new_state;
  event.held = button_old_state && button_new_state;
  return event;
}

KeyEvents TetrisClone::getKeyEvents(KeyStates &last_key_states) {
  KeyEvents ret_val;
  for (const auto &pair : key_bindings_) {
    const auto &key = pair.first;
    auto new_key_state = GetKey(pair.second).bHeld;
    ret_val[key] = getButtonState(last_key_states.at(key), new_key_state);
    last_key_states[key] = new_key_state;
  }
  return ret_val;
}

void resetDas(GameState<> &state) { state.das_counter = 0; }

void fullyChargeDas(GameState<> &state) { state.das_counter = 16; }

void processKeyEvents(const KeyEvents &key_events,
                      const sound::SoundPlayer &sample_player, GameState<>& state ) {
  auto move_check_wall_charge = [&sample_player](GameState<> &state, const int direction) {
    if (updateStateOnNoCollision(state.grid, direction, 0, 0, state.active_tetromino)) {
      sample_player.playSample("move");
    } else {
      fullyChargeDas(state);
    }
  };

  if (key_events.at(KeyAction::Left).pressed) {
    resetDas(state);
    move_check_wall_charge(state, -1);
  }
  if (key_events.at(KeyAction::Right).pressed) {
    resetDas(state);
    move_check_wall_charge(state, +1);
  }
  auto das_trigger = [](int &das_counter) {
    ++das_counter;
    if (das_counter >= 16) {
      das_counter = 10;
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
    gr = gr > 2 ? 2 : gr;
  }
  if (key_events.at(KeyAction::RotateLeft).pressed) {
    if (updateStateOnNoCollision(state.grid, 0, 0, 1, state.active_tetromino)) {
      sample_player.playSample("rotate");
    }
  }
  if (key_events.at(KeyAction::RotateRight).pressed) {
    if (updateStateOnNoCollision(state.grid, 0, 0, -1, state.active_tetromino)) {
      sample_player.playSample("rotate");
    }
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

void updateScore(const int line_clears, GameState<> &state) {
  state.score += state.level * line_scores[line_clears];
  state.lines += line_clears;
  state.lines_until_next_level -= line_clears;
  if (state.lines_until_next_level <= 0) {
    ++state.level;
    state.lines_until_next_level += 10;
  }
}

int getEntryDelayFromLockHeight(const int height) { return height * -0.5 + 19; }

bool didTetrominoLock(GameState<> &state) {
  if (--state.gravity_counter == 0) {
    state.gravity_counter = getGravity(state.level);
    if (not updateStateOnNoCollision(state.grid, 0, 1, 0, state.active_tetromino)) {
      state.entry_delay_counter = getEntryDelayFromLockHeight(state.active_tetromino.y);
      state.spawn_new_tetromino = true;
      state.grid = addTetrominoToGrid(state.grid, state.active_tetromino);
      return true;
    }
  }
  return false;
}

void TetrisClone::RenderNextTetromino(const Tetromino &next_tetromino, const int level) {
  auto get_plotting_coords = [](const Tetromino &next_tetromino) -> std::tuple<int, int> {
    constexpr int start_x = 196;
    constexpr int start_y = 104;
    int x, y;
    if (next_tetromino == Tetromino::Square) {
      return {start_x + 4, start_y + 8};
    } else if (next_tetromino == Tetromino::Line) {
      return {start_x - 4, start_y - 4};
    } else {
      return {start_x, start_y};
    }
  };

  // Clear existing tetromino.
  FillRect(191, 112, 33, 15, olc::BLACK);

  const auto tetromino = getTetrominoGrid(next_tetromino, 0);
  const auto [x, y] = get_plotting_coords(next_tetromino);
  RenderGrid(x, y, tetromino, level, 8, 8, getColor(next_tetromino));
}

void TetrisClone::DrawNumber(const int x, const int y, const int num, const int pad,
                             const olc::Pixel color) {
  FillRect(x, y, 8 * pad, 8, olc::BLACK);
  std::stringstream ss;
  ss << std::setw(pad) << std::setfill('0') << num;
  DrawString(x, y, ss.str(), color);
}

void TetrisClone::RenderText(const GameState<> &state) {
  DrawNumber(152, 16, state.lines, 3);
  DrawNumber(192, 32, state.high_score, 6);
  DrawNumber(192, 56, state.score, 6);
  DrawNumber(208, 160, state.level, 2);
  for (int i = 0; i < 7; ++i) {
    DrawNumber(48, 88 + (i * 16), state.tetromino_counts[i], 3, olc::RED);
  }
}

void TetrisClone::RenderDebug(const GameState<> &state) {
  auto get_das_color = [](const int &das) {
    if (das >= 10) {
      return olc::GREEN;
    } else if (das >= 6) {
      return olc::YELLOW;
    } else {
      return olc::RED;
    };
  };
  const auto color = get_das_color(state.das_counter);
  FillRect(200, 180, 3 * 16, 6, olc::BLACK);
  FillRect(200, 180, 3 * state.das_counter, 6, color);

  auto fill_circle = [&](const int x, const int y, const int radius, const bool signal,
                         const olc::Pixel &on_color, const olc::Pixel &off_color) {
    auto color = signal ? on_color : off_color;
    FillCircle(x, y, radius, color);
  };

  // fill_circle(194, 183, 3, state.das_reset_signal, olc::RED, olc::GREEN);

  DrawString(190, 189, "ARE:", olc::WHITE);
  fill_circle(226, 192, 3, entryDelay(state), olc::GREEN, olc::RED);

  DrawString(204, 199, "<  >", olc::WHITE);
  fill_circle(196, 202, 4, key_states_[KeyAction::Left], olc::GREEN, olc::BLACK);
  fill_circle(242, 201, 4, key_states_[KeyAction::Right], olc::GREEN, olc::BLACK);
  DrawString(204, 207, "A  B", olc::WHITE);
  fill_circle(196, 216, 8, key_states_[KeyAction::RotateRight], olc::GREEN, olc::BLACK);
  fill_circle(242, 216, 8, key_states_[KeyAction::RotateLeft], olc::GREEN, olc::BLACK);
}

void TetrisClone::RenderGameState(const GameState<> &state) {
  auto get_grid_for_render = [](const GameState<> &state) {
    if (entryDelay(state)) {
      return state.grid;
    } else {
      return addTetrominoToGrid(state.grid, state.active_tetromino);
    }
  };
  // Clear the field.
  FillRect(96, 40, 80, 160, olc::BLACK);
  DrawSprite(13, 61, counter_sprites_[state.level % 10].get());
  RenderGrid(96, 40, get_grid_for_render(state), state.level);
  RenderNextTetromino(state.next_tetromino, state.level);
  RenderText(state);
  if (debug_mode_) {
    RenderDebug(state);
  }
}

// TODO:: This can be broken into two functions:
//          -Apply clear line animation to state (free float)
//          -Render grid                         (class)
void TetrisClone::RenderLineClearAnimation(GameState<> &state,
                                           LineClearAnimationInfo &line_clear_info) {
  auto &frame = line_clear_info.animation_frame;
  if (frame == 0) {
    return;
  }
  --frame;

  if (frame == 23) {
    if (line_clear_info.rows.size() == 4) {
      sample_player_.playSample("tetris");
    } else if (line_clear_info.rows.size() > 0) {
      sample_player_.playSample("line_clear");
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
  // If it was a tetris, do the flash.
  if ((line_clear_info.rows.size() == 4) && ((frame - 1) % 4 == 0)) {
    DrawSprite(0, 0, bg_flash_ptr_.get());
  } else {
    DrawSprite(0, 0, bg_ptr_.get());
  }
}

Tetromino TetrisClone::getRandomTetromino() { return Tetromino(random_generator_(real_rng_)); }

void TetrisClone::spawnNewTetromino(GameState<> &state) {
  state.active_tetromino = {state.next_tetromino, 5, 0, 0};
  if (tetrominoCollision(state.grid, state.active_tetromino)) {
    state.game_over = true;
  }
  state_.tetromino_counts[static_cast<int>(state_.active_tetromino.tetromino)]++;
  state.next_tetromino = getRandomTetromino();
  state.spawn_new_tetromino = false;
}

int linesToClearFromStartingLevel(const int level) {
  if (level < 10) {
    return level * 10 + 10;
  } else {
    return std::max(100, level * 10 - 50);
  }
}

GameState<> TetrisClone::getNewState(const int level) {
  auto state = GameState<>{};
  state.level = level;
  state.active_tetromino = {getRandomTetromino(), 5, 0, 0};
  state.next_tetromino = getRandomTetromino();
  state.tetromino_counts[static_cast<int>(state_.active_tetromino.tetromino)]++;
  state.gravity_counter = GRAVITY_FIRST_FRAME;
  state.lines_until_next_level = linesToClearFromStartingLevel(state.level);
  return state;
}

// TODO:: Add asset loading to constructor/OnUserCreate and add error
// handling.
TetrisClone::TetrisClone(const int start_level)
    : bg_ptr_{std::make_unique<olc::Sprite>("assets/images/basic_field_empty.png")},
      bg_flash_ptr_{std::make_unique<olc::Sprite>("assets/images/basic_field_flash.png")},
      block_sprites_{loadSprites("assets/images")},
      counter_sprites_{loadCounterSprites("assets/images")}, state_{}, real_rng_{}, fake_rng_{},
      random_generator_(0, 6), key_states_{}, key_bindings_{getKeyBindings()}, debug_mode_{true},
      frame_end_{}, line_clear_info_{}, sample_player_{} {
  sAppName = "TetrisClone";
  for (const auto &pair : key_bindings_) {
    key_states_[pair.first] = false;
  }
  state_ = getNewState(start_level);
}

bool TetrisClone::OnUserCreate() {
  if (ScreenWidth() != 256 || ScreenHeight() != 225) {
    LOG_ERROR("Screen size must be set to 256x240 for this application.");
    return false;
  }
  DrawSprite(0, 0, bg_ptr_.get());
  frame_end_ = Clock::now() + single_frame;

  const std::string path{"./assets/sounds/"};
  bool ret = true;
  ret = ret && sample_player_.loadWavFromFilesystem(path + "tetromino_move.wav", "move");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "tetromino_lock.wav", "lock");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "tetromino_rotate.wav", "rotate");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "tetris.wav", "tetris");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "line_clear.wav", "line_clear");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "menu_blip.wav", "menu_blip");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "menu_select_01.wav", "menu_select_01");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "menu_select_02.wav", "menu_select_02");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "pause.wav", "pause");
  ret = ret && sample_player_.loadWavFromFilesystem(path + "top_out.wav", "top_out");

  return ret;
}

bool TetrisClone::OnUserUpdate(float fElapsedTime) {
  if (not entryDelay(state_)) {
    if (state_.spawn_new_tetromino && not state_.game_over) {
      spawnNewTetromino(state_);
    }
    const auto key_events = getKeyEvents(key_states_);

    processKeyEvents(key_events, sample_player_, state_);
    if (didTetrominoLock(state_)) {
      sample_player_.playSample("lock");
      auto lines_cleared = checkForLineClears(state_);
      if (not lines_cleared.empty()) {
        // Emulate a quirk in the nes implementation: The animation would only
        // start on certain frames, randomly increasing/decreasing the ARE.
        // TODO:: -use modern c++ random and hold the random seed in class state
        //        -also put it in a function
        state_.entry_delay_counter += (17 + (rand() % 5));
        line_clear_info_ = LineClearAnimationInfo{lines_cleared, state_.entry_delay_counter};
      }
    }
    RenderGameState(state_);
  } else {
    RenderLineClearAnimation(state_, line_clear_info_);
    // When the animation is almost over, update the score.
    if (line_clear_info_.animation_frame == 4) {
      updateScore(line_clear_info_.rows.size(), state_);
    }
    getKeyEvents(key_states_);
    RenderGameState(state_);
    --state_.entry_delay_counter;
  }

  std::this_thread::sleep_until(frame_end_);
  frame_end_ += single_frame;
  if (GetKey(olc::Key::Q).bHeld) {
    return false;
  } else {
    return true;
  }
}

/**
 * TODO:
 * - Clean up sound implementation!
 *      - Hide SDL behind an interface (it's ugly)
 *      - Proper initialization/loading/playing of sounds
 *      - Add remaining sounds (level up sample missing)
 *      - No sound on blocked rotation
 *      - Synchronize line clears properly (it's pretty close i think)
 *
 * - Top out/game over
 * - Some kind of menu system
 * - Asset loading from binary
 * - Press down scoring
 * - Check rotation consistency/tetromino entry state is correct
 *
 * IDEAS::
 * - Wall charge animation/signal
 * - Das chain counter/animation
 * - Interface for random generator: allow a tetromino set loader
 *      (don't forget the rand() in line clear)
 * - Record all inputs, implement a replay functionality
 *
 * REFACTOR::
 * - Move everything 'gamestate' related into it's own hpp/cpp
 * - Move all tetromino defs and helper functions into their own hpp/cpp
 */

} // namespace tetris_clone
