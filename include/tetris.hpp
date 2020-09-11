#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "olcPixelGameEngine.h"
#include "sound.hpp"
#include "tetromino.hpp"

namespace tetris_clone {

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
        tetromino_counts{},
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
  std::array<int, 7> tetromino_counts; // Ordered according to enum.
  bool topped_out;
  bool paused;
};

struct KeyEvent {
  bool pressed{};
  bool released{};
  bool held{};
};

enum class KeyAction { Up, Down, Left, Right, RotateLeft, RotateRight, Start, Quit };

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

private:
  inline olc::Sprite *getSprite(const std::string &sprite_name) {
    return sprite_map_.at(sprite_name).get();
  }
  inline olc::Sprite *getBlockSprite(const int level, const int color) {
    return block_sprites_.at(level % 10).at(color).get();
  }

  void RenderGameState(const GameState<> &state);
  void RenderNextTetromino(const Tetromino &next_tetromino, const int level);
  void DrawNumber(const int x, const int y, const int num, const int pad,
                  const olc::Pixel color = olc::WHITE);
  void RenderText(const GameState<> &state);
  void RenderDebug(const GameState<> &state);
  void RenderLineClearAnimation(GameState<> &state, LineClearAnimationInfo &line_clear_info_);
  void RenderPaused();

  template <typename GridContainer>
  void RenderGrid(const int x_start, const int y_start, const GridContainer &grid, const int level,
                  const int x_spacing = 8, const int y_spacing = 8, const int color = 1) {
    for (int i = 0; i < grid.size(); ++i) {
      for (int j = 0; j < grid[i].size(); ++j) {
        if (grid[i][j] == 0) {
          continue;
        }
        DrawSprite(x_start + i * x_spacing, y_start + j * y_spacing,
                   getBlockSprite(level, grid[i][j] * color));
      }
    }
  }

  bool spawnNewTetromino(GameState<> &state);
  Tetromino getRandomTetromino();
  KeyEvents getKeyEvents(KeyStates &current_key_state);
  GameState<> getNewState(const int level);

  std::vector<std::vector<std::unique_ptr<olc::Sprite>>> block_sprites_;
  std::map<std::string, std::unique_ptr<olc::Sprite>> sprite_map_;
  GameState<> state_;
  std::random_device real_rng_;
  std::default_random_engine fake_rng_;
  std::uniform_int_distribution<int> random_generator_;
  KeyStates key_states_;
  KeyBindings key_bindings_;
  bool debug_mode_;
  std::chrono::time_point<std::chrono::high_resolution_clock> frame_end_;
  LineClearAnimationInfo line_clear_info_;
  int top_out_frame_counter_;
  sound::SoundPlayer sample_player_;
};

} // namespace tetris_clone
