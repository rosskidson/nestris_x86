
#include "frame_processors/game_processor.hpp"

#include <stdexcept>

#include "assets.hpp"
#include "game_logic.hpp"
#include "gravity.hpp"
#include "key_defines.hpp"
#include "utils/logging.hpp"
#include "sound.hpp"

namespace tetris_clone {

constexpr int GRAVITY_FIRST_FRAME = 100;

GameProcessor::GameProcessor(const GameOptions& options, const std::shared_ptr<Renderer>& renderer,
                             const std::shared_ptr<sound::SoundPlayer>& sample_player)
    : renderer_(renderer),
      sample_player_(sample_player),
      state_{},
      real_rng_{},
      random_generator_{0, 6},
      das_processor_{options.das_full_charge, options.das_min_charge},
      gravity_provider_{options.gravity_type},
      show_controls_{options.show_controls},
      show_das_bar_{options.show_das_bar},
      show_entry_delay_{options.show_entry_delay},
      line_clear_info_{},
      top_out_frame_counter_{} {
  state_ = getNewState(options.level);
}

Tetromino GameProcessor::getRandomTetromino() {
  return Tetromino(random_generator_(real_rng_));
}

bool GameProcessor::spawnNewTetromino(GameState<>& state) {
  state.active_tetromino = {state.next_tetromino, 5, 0, 0};
  state_.tetromino_counts[static_cast<int>(state_.active_tetromino.tetromino)]++;
  state.next_tetromino = getRandomTetromino();
  state.spawn_new_tetromino = false;
  return not tetrominoCollision(state.grid, state.active_tetromino);
}

GameState<> GameProcessor::getNewState(const int level) {
  auto state = GameState<>{};
  state.level = level;
  state.active_tetromino = {getRandomTetromino(), 5, 0, 0};
  state.next_tetromino = getRandomTetromino();
  state.tetromino_counts[static_cast<int>(state.active_tetromino.tetromino)]++;
  state.gravity_counter = GRAVITY_FIRST_FRAME;
  state.lines_until_next_level = linesToClearFromStartingLevel(state.level);
  return state;
}

void GameProcessor::doGravityStep(const KeyEvents& key_events) {
  if (state_.spawn_new_tetromino) {
    const bool topped_out = not spawnNewTetromino(state_);
    if (topped_out) {
      // Lock the active tetromino and move it off the grid. This is to stop the active
      // tetromino interfering with the 'curtain' animation.
      state_.grid = addTetrominoToGrid(state_.grid, state_.active_tetromino);
      state_.active_tetromino.y = -10;
      state_.topped_out = true;
      sample_player_->playSample("top_out");
    }
  }

  processKeyEvents(key_events, *sample_player_, das_processor_, state_);

  const bool tetromino_locked = applyGravity(gravity_provider_, state_);
  if (tetromino_locked) {
    sample_player_->playSample("lock");
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
    renderer_->doTetrisFlash(line_clear_info_.animation_frame);
  }
  // When the animation is almost over, update the score.
  if (line_clear_info_.animation_frame == 4) {
    updateScoreAndLevel(line_clear_info_.rows.size(), *sample_player_, state_);
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
  renderer_->renderGameState(state_, show_controls_, show_das_bar_, show_entry_delay_, key_events,
                             das_processor_);
  return ProgramFlowSignal::FrameSuccess;
}

};  // namespace tetris_clone
