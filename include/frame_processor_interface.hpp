#pragma once

#include "key_defines.hpp"

namespace tetris_clone {

enum class ProgramFlowSignal {
  FrameSuccess,            // The frame processor intends to run again on the next frame.
  StartGame,               // A new game should be created and a game processor should start.
  OptionsScreen,           // Open options screen.
  LevelSelectorScreen,     // Open level selector screen.
  EndProgram               // End the program.
};

class FrameProcessorInterface {

  /**
   * @brief Process a single frame
   *
   * @param key_events  A data structure containing all the key events for this frame
   * @return A signal pertaining to program flow.
   */
  public:
    virtual ProgramFlowSignal processFrame(const KeyEvents& key_events) = 0;

    virtual ~FrameProcessorInterface() {}

};
}
