#pragma once

#include <memory>

#include "assets.hpp"
#include "frame_processor_interface.hpp"
#include "pixel_drawing_interface.hpp"
#include "sound.hpp"

namespace tetris_clone {

class LevelScreenProcessor : public FrameProcessorInterface {
 public:
  LevelScreenProcessor(std::unique_ptr<PixelDrawingInterface>&& drawer,
                       const std::shared_ptr<sound::SoundPlayer>& sample_player,
                       const std::shared_ptr<SpriteProvider>& sprite_provider);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

  int getSelectedLevel() const;

 private:
  ProgramFlowSignal processKeyEvents(const KeyEvents& key_events,
                                     const sound::SoundPlayer& sample_player_);
  void renderMenu() const;
  void renderLevelSelector(const int level) const;

  std::unique_ptr<PixelDrawingInterface> drawer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  int level_;
  bool options_selected_;
  bool plus_ten_levels_;
  std::unique_ptr<std::atomic_int> frame_counter_;
};

}  // namespace tetris_clone
