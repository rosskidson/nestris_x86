#pragma once

#include <random>

#include "frame_processor_interface.hpp"
#include "game_states.hpp"
#include "rendering.hpp"
#include "sound.hpp"

namespace tetris_clone {

// struct LineClearAnimationInfo {
//  std::vector<int> rows;
//  int animation_frame{};
//};

class GameProcessor : public FrameProcessorInterface {
 public:
  GameProcessor(const GameOptions& options, const std::shared_ptr<Renderer>& renderer);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

 private:
  bool spawnNewTetromino(GameState<>& state);
  Tetromino getRandomTetromino();
  GameState<> getNewState(const int level);

  std::shared_ptr<Renderer> renderer_;
  sound::SoundPlayer sample_player_;
  GameState<> state_;
  std::random_device real_rng_;
  std::uniform_int_distribution<int> random_generator_;
  bool show_controls_;
  LineClearAnimationInfo line_clear_info_;
  int top_out_frame_counter_;
};

}  // namespace tetris_clone
