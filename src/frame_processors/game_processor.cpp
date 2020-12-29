
#include "frame_processors/game_processor.hpp"

#include <stdexcept>

#include "assets.hpp"
#include "drawers/pixel_drawing_interface.hpp"
#include "game_logic.hpp"
#include "gravity.hpp"
#include "key_defines.hpp"
#include "sound.hpp"
#include "utils/logging.hpp"

namespace nestris_x86 {

constexpr int GRAVITY_FIRST_FRAME = 100;

GameProcessor::GameProcessor(const GameOptions& options,
                             std::unique_ptr<PixelDrawingInterface>&& drawer,
                             const std::shared_ptr<sound::SoundPlayer>& sample_player,
                             const std::shared_ptr<SpriteProvider>& sprite_provider)
    : renderer_(std::move(drawer), sprite_provider, "./assets/images"),
      sample_player_(sample_player),
      state_{},
      statistics_{},
      real_rng_{},
      random_generator_{0, 6},
      das_processor_{options.das_full_charge, options.das_min_charge},
      gravity_provider_{options.gravity_type},
      show_controls_{options.show_controls},
      show_das_bar_{options.show_das_bar},
      statistics_mode_{options.statistics_mode},
      line_clear_info_{},
      top_out_frame_counter_{} {
  state_ = getNewState(options.level);
  statistics_.update(state_.active_tetromino.tetromino);
}

void GameProcessor::reset(const GameOptions& options) {
  das_processor_ = Das{options.das_full_charge, options.das_min_charge};
  gravity_provider_ = Gravity{options.gravity_type};
  show_controls_ = options.show_controls;
  show_das_bar_ = options.show_das_bar;
  statistics_mode_ = options.statistics_mode;
  line_clear_info_ = {};
  top_out_frame_counter_ = {};
  state_ = getNewState(options.level);
  statistics_ = {};
  statistics_.update(state_.active_tetromino.tetromino);
  renderer_.startNewGame();
}

Tetromino GameProcessor::getRandomTetromino() {
  return Tetromino(random_generator_(real_rng_));
}

bool GameProcessor::spawnNewTetromino(GameState<>& state) {
  state.active_tetromino = {state.next_tetromino, 5, 0, 0};
  // state_.tetromino_counts[static_cast<int>(state_.active_tetromino.tetromino)]++;
  state.next_tetromino = getRandomTetromino();
  state.spawn_new_tetromino = false;
  return not tetrominoCollision(state.grid, state.active_tetromino);
}

GameState<> GameProcessor::getNewState(const int level) {
  auto state = GameState<>{};
  state.level = level;
  state.active_tetromino = {getRandomTetromino(), 5, 0, 0};
  state.next_tetromino = getRandomTetromino();
  state.gravity_counter = GRAVITY_FIRST_FRAME;
  state.lines_until_next_level = linesToClearFromStartingLevel(state.level);
  return state;
}

void GameProcessor::doGravityStep(const KeyEvents& key_events) {
  if (state_.spawn_new_tetromino) {
    const bool topped_out = not spawnNewTetromino(state_);
    statistics_.update(state_.active_tetromino.tetromino);
    if (topped_out) {
      // Lock the active tetromino and move it off the grid. This is to stop the active
      // tetromino interfering with the 'curtain' animation.
      state_.grid = addTetrominoToGrid(state_.grid, state_.active_tetromino);
      state_.active_tetromino.y = -10;
      state_.topped_out = true;
      sample_player_->playSample("top_out");
    }
  }

  processKeyEvents(key_events, *sample_player_, das_processor_, state_, statistics_);
  if(not das_processor_.dasSoftlyCharged(state_.das_counter)) {
    statistics_.dasResetSignal();
  }

  const bool tetromino_locked = applyGravity(key_events, gravity_provider_, state_);
  if (tetromino_locked) {
    sample_player_->playSample("tetromino_lock");
    state_.press_down_lock = true;
    addPressDownScore(state_);
    auto lines_cleared = checkForLineClears(state_);
    if (not lines_cleared.empty()) {
      updateEntryDelayForLineClear(state_.entry_delay_counter);
      line_clear_info_ = LineClearAnimationInfo{lines_cleared, state_.entry_delay_counter};
    }
  }
}

void GameProcessor::doEntryDelayStep(const KeyEvents& key_events) {
  if (key_events.at(KeyAction::Start).pressed) {
    state_.paused = true;
    sample_player_->playSample("pause");
  }

  animateLineClear(*sample_player_, state_, line_clear_info_);
  if (line_clear_info_.rows.size() == 4) {
    renderer_.doTetrisFlash(line_clear_info_.animation_frame);
  }
  // When the animation is almost over, update the score.
  if (line_clear_info_.animation_frame == 4) {
    updateScoreAndLevel(line_clear_info_.rows.size(), *sample_player_, state_);
    statistics_.update(line_clear_info_.rows.size(), state_.level);
  }
  --state_.entry_delay_counter;
}

ProgramFlowSignal GameProcessor::processFrame(const KeyEvents& key_events) {
  if (state_.topped_out) {
    const bool end_game = updateTopOutState(key_events, top_out_frame_counter_, state_);
    if (end_game) {
      return ProgramFlowSignal::LevelSelectorScreen;
    }
  } else if (state_.paused) {
    if (key_events.at(KeyAction::Start).pressed) {
      state_.paused = false;
    }
  } else if (entryDelay(state_)) {
    doEntryDelayStep(key_events);
  } else {
    doGravityStep(key_events);
  }
  renderer_.renderGameState(state_, statistics_, show_controls_, show_das_bar_, statistics_mode_,
                            key_events, das_processor_);
  return ProgramFlowSignal::FrameSuccess;
}

};  // namespace nestris_x86
