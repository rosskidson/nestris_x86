#pragma once

#include "frame_processor_interface.hpp"
#include "game_states.hpp"

namespace tetris_clone {

class GameProcessor : public FrameProcessorInterface {
 public:
  GameProcessor(const GameOptions& options);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

 private:
  bool spawnNewTetromino(GameState<>& state);
  Tetromino getRandomTetromino();
  KeyEvents getKeyEvents(KeyStates& current_key_state);
  GameState<> getNewState(const int level);

  Renderer renderer_;  // Make a `BasicRenderer` for menu and `TetrisRenderer (derived from basic)
  sound::SoundPlayer sample_player_;
  GameState<> state_;
  std::random_device real_rng_;
  std::uniform_int_distribution<int> random_generator_;
  bool debug_mode_;
  LineClearAnimationInfo line_clear_info_;
  int top_out_frame_counter_;
};

}  // namespace tetris_clone
