#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "olcPixelGameEngine.h"
#include "sound.hpp"
#include "tetromino.hpp"
#include "rendering.hpp"
#include "game_state.hpp"
#include "key_defines.hpp"

namespace tetris_clone {

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
  bool runMenuSingleFrame();
  bool runGameSingleFrame();
  void sleepUntilNextFrame();

  bool spawnNewTetromino(GameState<> &state);
  Tetromino getRandomTetromino();
  KeyEvents getKeyEvents(KeyStates &current_key_state);
  GameState<> getNewState(const int level);

  Renderer renderer_;
  sound::SoundPlayer sample_player_;
  GameState<> state_;
  bool menu_;
  std::random_device real_rng_;
  std::uniform_int_distribution<int> random_generator_;
  KeyStates key_states_;
  KeyBindings key_bindings_;
  bool debug_mode_;
  std::chrono::time_point<std::chrono::high_resolution_clock> frame_end_;
  LineClearAnimationInfo line_clear_info_;
  int top_out_frame_counter_;
};

} // namespace tetris_clone
