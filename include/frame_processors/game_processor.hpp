#pragma once

#include <random>

#include "assets.hpp"
#include "das.hpp"
#include "frame_processor_interface.hpp"
#include "game_states.hpp"
#include "gravity.hpp"
#include "pixel_drawing_interface.hpp"
#include "rendering.hpp"
#include "sound.hpp"

namespace tetris_clone {

struct LineClearAnimationInfo {
  std::vector<int> rows;
  int animation_frame{};
};

class GameProcessor : public FrameProcessorInterface {
 public:
  GameProcessor(const GameOptions& options, std::unique_ptr<PixelDrawingInterface>&& drawer,
                const std::shared_ptr<sound::SoundPlayer>& sample_player_,
                const std::shared_ptr<SpriteProvider>& sprite_provider);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

 private:
  bool spawnNewTetromino(GameState<>& state);
  Tetromino getRandomTetromino();
  GameState<> getNewState(const int level);

  void doGravityStep(const KeyEvents& key_events);
  void doEntryDelayStep(const KeyEvents& key_events);

  Renderer renderer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  GameState<> state_;
  std::random_device real_rng_;
  std::uniform_int_distribution<int> random_generator_;
  Das das_processor_;
  Gravity gravity_provider_;
  bool show_controls_;
  bool show_das_bar_;
  bool show_entry_delay_;
  LineClearAnimationInfo line_clear_info_;
  int top_out_frame_counter_;
};

}  // namespace tetris_clone
