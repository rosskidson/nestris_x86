#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "olcPixelGameEngine.h"

namespace tetris_clone {

enum class Tetromino { T, J, Z, Square, S, L, Line };

struct TetrominoState {
  Tetromino tetromino;
  int x;
  int y;
  int rotation;
};

template <int W = 10, int H = 20>
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
        tetromino_counts{},
        game_over{} {}

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
  std::array<int, 7> tetromino_counts;  // Ordered according to enum.
  bool game_over;
};

struct KeyEvent {
  bool pressed{};
  bool released{};
  bool held{};
};

enum class KeyAction {
  Up,
  Down,
  Left,
  Right,
  RotateLeft,
  RotateRight,
  Start,
  Quit
};

using KeyBindings = std::map<KeyAction, olc::Key>;
using KeyStates = std::map<KeyAction, bool>;
using KeyEvents = std::map<KeyAction, KeyEvent>;

struct LineClearAnimationInfo {
  std::vector<int> rows;
  int animation_frame{};
};

class TetrisClone : public olc::PixelGameEngine {
 public:
  TetrisClone(const int start_level = 0);

  bool OnUserCreate() override;

  bool OnUserUpdate(float fElapsedTime) override;

  // void setStartLevel(const int level);

 private:
  void RenderGameState(const GameState<>& state);
  void RenderNextTetromino(const Tetromino& next_tetromino, const int level);
  void DrawNumber(const int x, const int y, const int num, const int pad,
                  const olc::Pixel color = olc::WHITE);
  void RenderText(const GameState<>& state);
  void RenderDebug(const GameState<>& state);
  void RenderLineClearAnimation(GameState<>& state,
                                LineClearAnimationInfo& line_clear_info_);

  template <typename GridContainer>
  void RenderGrid(const int x_start, const int y_start,
                  const GridContainer& grid, const int level,
                  const int x_spacing = 8, const int y_spacing = 8,
                  const int color = 1) {
    for (int i = 0; i < grid.size(); ++i) {
      for (int j = 0; j < grid[i].size(); ++j) {
        if (grid[i][j] == 0) {
          continue;
        }
        DrawSprite(x_start + i * x_spacing, y_start + j * y_spacing,
                   block_sprites_[level % 10][grid[i][j] * color].get());
      }
    }
  }

  void spawnNewTetromino(GameState<>& state);
  Tetromino getRandomTetromino();
  KeyEvents getKeyEvents(KeyStates& current_key_state);
  GameState<> getNewState(const int level);

  std::unique_ptr<olc::Sprite> bg_ptr_;
  std::unique_ptr<olc::Sprite> bg_flash_ptr_;
  std::vector<std::vector<std::unique_ptr<olc::Sprite>>> block_sprites_;
  std::vector<std::unique_ptr<olc::Sprite>> counter_sprites_;
  GameState<> state_;
  std::random_device real_rng_;
  std::default_random_engine fake_rng_;
  std::uniform_int_distribution<int> random_generator_;
  KeyStates key_states_;
  KeyBindings key_bindings_;
  bool debug_mode_;
  std::chrono::time_point<std::chrono::high_resolution_clock> frame_end_;
  LineClearAnimationInfo line_clear_info_;
};

}  // namespace tetris_clone
