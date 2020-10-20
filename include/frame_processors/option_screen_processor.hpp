#pragma once

#include <memory>
#include <set>

#include "assets.hpp"
#include "frame_processor_interface.hpp"
#include "option.hpp"
#include "pixel_drawing_interface.hpp"
#include "sound.hpp"

namespace tetris_clone {

class OptionScreenProcessor : public FrameProcessorInterface {
 public:
  using OptionMap = std::map<std::string, std::unique_ptr<OptionInterface>>;

  OptionScreenProcessor(std::unique_ptr<PixelDrawingInterface>&& drawer,
                        const std::shared_ptr<sound::SoundPlayer>& sample_player,
                        const std::shared_ptr<SpriteProvider>& sprite_provider);

  ProgramFlowSignal processFrame(const KeyEvents& key_events);

  const OptionMap& getOptions() { return options_; }

 private:
  ProgramFlowSignal processKeyEvents(const KeyEvents& key_events);

  void setDasProfile();

  void renderOptionScreen() const;

  std::vector<int> renderOptions(const std::set<int>& spacers, const int left_column,
                                 const int right_column, const int first_row,
                                 const bool grey_out_das_options) const;

  void renderSelector(const int column_location, const std::vector<int>& row_locations) const;

  inline const OptionInterface& getSelectedOption() const {
    return *options_.at(option_order_.at(selected_index_));
  }

  inline OptionInterface& getSelectedOption() {
    return *options_.at(option_order_.at(selected_index_));
  }

  inline bool isSelectedOptionDummy() const {
    return dynamic_cast<const DummyOption*>(options_.at(option_order_.at(selected_index_)).get());
  }

  std::unique_ptr<PixelDrawingInterface> drawer_;
  std::shared_ptr<sound::SoundPlayer> sample_player_;
  std::shared_ptr<SpriteProvider> sprite_provider_;
  OptionMap options_;
  std::vector<std::string> option_order_;
  int selected_index_;
  bool grey_out_das_options_;

  // frame_counter_ is incremented in a const function.
  // The unique pointer provides an indirection to allow mutation within a const function
  // The std::atomic is to keep it thread safe, as per the requirement of const functions.
  std::unique_ptr<std::atomic_int> frame_counter_;
};

}  // namespace tetris_clone
