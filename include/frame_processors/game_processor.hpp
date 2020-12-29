#pragma once

#include <random>

#include "assets.hpp"
#include "das.hpp"
#include "drawers/pixel_drawing_interface.hpp"
#include "frame_processor_interface.hpp"
#include "game_renderer.hpp"
#include "game_states.hpp"
#include "gravity.hpp"
#include "sound.hpp"
#include "statistics.hpp"
#include "tetris_type.hpp"

namespace nestris_x86 {

struct GameOptions {
  int level{};
  int das_full_charge{Das::NTSC_FULL_CHARGE};
  int das_min_charge{Das::NTSC_MIN_CHARGE};
  int game_frequency{NTSC_FREQUENCY};
  TetrisType gravity_type{TetrisType::NTSC};
  bool show_controls{true};
  bool show_das_bar{true};
  StatisticsMode statistics_mode{};
};

class GameProcessor : public FrameProcessorInterface {
 public:
  GameProcessor(const GameOptions& options, std::unique_ptr<PixelDrawingInterface>&& drawer,
                const std::shared_ptr<sound::SoundPlayer>& sample_player_,
                const std::shared_ptr<SpriteProvider>& sprite_provider);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

  void reset(const GameOptions& options);

 private:
  bool spawnNewTetromino(GameState<>& state);
  Tetromino getRandomTetromino();
  GameState<> getNewState(const int level);

  void doGravityStep(const KeyEvents& key_events);
  void doEntryDelayStep(const KeyEvents& key_events);

  GameRenderer renderer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  GameState<> state_;
  Statistics statistics_;
  std::random_device real_rng_;
  std::uniform_int_distribution<int> random_generator_;
  Das das_processor_;
  Gravity gravity_provider_;
  bool show_controls_;
  bool show_das_bar_;
  StatisticsMode statistics_mode_;
  LineClearAnimationInfo line_clear_info_;
  int top_out_frame_counter_;
};

}  // namespace nestris_x86
