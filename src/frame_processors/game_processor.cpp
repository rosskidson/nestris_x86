
#include "game_processor.hpp"

#include <stdexcept>

#include "assets.hpp"
#include "game_logic.hpp"
#include "key_defines.hpp"
#include "logging.hpp"

namespace tetris_clone {

constexpr int GRAVITY_FIRST_FRAME = 100;

GameProcessor::GameProcessor(const GameOptions& options, const std::shared_ptr<Renderer>& renderer)
    : renderer_(renderer),
      sample_player_{},
      state_{},
      real_rng_{},
      random_generator_{0, 6},
      show_controls_{options.show_controls},
      line_clear_info_{},
      top_out_frame_counter_{} {
  if (not loadSoundAssets("./assets/sounds/", sample_player_)) {
    throw std::runtime_error("Failed loading sound samples.");
  }
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
      sample_player_.playSample("top_out");
    }
  }

  processKeyEvents(key_events, sample_player_, state_);

  const bool tetromino_locked = applyGravity(state_);
  if (tetromino_locked) {
    sample_player_.playSample("lock");
    auto lines_cleared = checkForLineClears(state_);
    if (not lines_cleared.empty()) {
      updateEntryDelayForLineClear(state_.entry_delay_counter);
      line_clear_info_ = LineClearAnimationInfo{lines_cleared, state_.entry_delay_counter};
    }
  }
}

void GameProcessor::doEntryDelayStep(const KeyEvents& key_events) {
  animateLineClear(sample_player_, state_, line_clear_info_);
  if (line_clear_info_.rows.size() == 4) {
    renderer_->doTetrisFlash(line_clear_info_.animation_frame);
  }
  // When the animation is almost over, update the score.
  if (line_clear_info_.animation_frame == 4) {
    updateScoreAndLevel(line_clear_info_.rows.size(), sample_player_, state_);
  }
  --state_.entry_delay_counter;
}

ProgramFlowSignal GameProcessor::processFrame(const KeyEvents& key_events) {
  if (state_.topped_out) {
    const bool end_game = updateTopOutState(key_events, top_out_frame_counter_, state_);
    if (end_game) {
      return ProgramFlowSignal::EndProgram;
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
  renderer_->renderGameState(state_, show_controls_, key_events);
  return ProgramFlowSignal::FrameSuccess;
}

};  // namespace tetris_clone
