
#include "frame_processors/keyboard_config_processor.hpp"

#include "drawing_utils.hpp"
#include "utils/logging.hpp"

namespace nestris_x86 {

using pdi = PixelDrawingInterface;

KeyboardConfigProcessor::KeyboardConfigProcessor(
    std::unique_ptr<PixelDrawingInterface>&& drawer,
    const std::shared_ptr<sound::SoundPlayer>& sample_player,
    std::shared_ptr<InputInterface>& input_interface, const KeyBindings& initial_bindings)
    : sample_player_(sample_player),
      key_bindings_(initial_bindings),
      default_bindings_(initial_bindings),
      active_key_{KeyAction::COUNT},
      selector_idx_{},
      wait_until_key_lifted_{true},
      keybinding_active_{},
      frame_counter_{std::make_unique<std::atomic_int>(0)},
      drawer_(std::move(drawer)),
      input_ptr_(input_interface) {}

ProgramFlowSignal KeyboardConfigProcessor::processKeyEvents(const KeyEvents& key_events) {
  if (key_events.at(KeyAction::Up).pressed) {
    sample_player_->playSample("menu_blip");
    selector_idx_ = selector_idx_ > 0 ? selector_idx_ - 1 : 2;
  }
  if (key_events.at(KeyAction::Down).pressed) {
    sample_player_->playSample("menu_blip");
    selector_idx_ = selector_idx_ < 2 ? selector_idx_ + 1 : 0;
  }
  if (key_events.at(KeyAction::RotateClockwise).pressed ||
      key_events.at(KeyAction::Start).pressed) {
    sample_player_->playSample("menu_select_02");
    if (selector_idx_ == 0) {
      key_bindings_ = {};
      keybinding_active_ = true;
      active_key_ = static_cast<KeyAction>(0);
      wait_until_key_lifted_ = true;
    }
    if (selector_idx_ == 1) {
      key_bindings_ = default_bindings_;
    }
    if (selector_idx_ == 2) {
      return ProgramFlowSignal::OptionsScreen;
    }
  }
  return ProgramFlowSignal::FrameSuccess;
}

void KeyboardConfigProcessor::clearScreen() const {
  constexpr pdi::Coords top_left{30, 30};
  constexpr pdi::Rect screen_size{197, 180};
  drawer_->fillRect(top_left, screen_size, pdi::BLACK());
}

void KeyboardConfigProcessor::renderKeyboardConfigScreen() const {
  clearScreen();
  constexpr int x_left_column = 32;
  constexpr int x_right_column = 180;

  int y_row = 40;
  for (const auto& [action, key] : key_bindings_) {
    drawer_->drawString(x_left_column, y_row, keyActionToString(action));
    drawer_->drawString(x_right_column, y_row, input_ptr_->keyCodeToStr(key));
    y_row += 10;
  }

  if (active_key_ < KeyAction::COUNT) {
    drawer_->drawString(x_left_column, y_row, keyActionToString(active_key_));
    drawer_->drawString(x_right_column, y_row, "???");
  }

  constexpr pdi::Coords menu_items_start{80, 150};
  drawer_->drawString(menu_items_start, "CHANGE CONFIG");
  drawer_->drawString(menu_items_start + pdi::Coords{20, 10}, "DEFAULTS");
  drawer_->drawString(menu_items_start + pdi::Coords{30, 20}, "ACCEPT");

  const auto selector_color = (*frame_counter_)++ % 4 ? pdi::WHITE() : pdi::BLACK();
  drawTriangleSelector(*drawer_, menu_items_start.x - 15, menu_items_start.y + (selector_idx_ * 10),
                       7, selector_color, false);
  drawTriangleSelector(*drawer_, menu_items_start.x + 120,
                       menu_items_start.y + (selector_idx_ * 10), 7, selector_color, true);
}

ProgramFlowSignal KeyboardConfigProcessor::processFrame(const KeyEvents& key_events) {
  if (wait_until_key_lifted_) {
    wait_until_key_lifted_ = input_ptr_->getPressedKey() != input_ptr_->getNullKey();
    return ProgramFlowSignal::FrameSuccess;
  }

  ProgramFlowSignal return_signal{ProgramFlowSignal::FrameSuccess};
  if (keybinding_active_) {
    const auto pressed_key = input_ptr_->getPressedKey();
    if (pressed_key != input_ptr_->lookupKeyCode("NONE")) {
      for (auto& [action, key] : key_bindings_) {
        if (key == pressed_key) {
          key = input_ptr_->getNullKey();
        }
      }
      key_bindings_[active_key_] = pressed_key;
      wait_until_key_lifted_ = true;
      active_key_ = static_cast<KeyAction>(static_cast<int>(active_key_) + 1);
      if (active_key_ == KeyAction::COUNT) {
        keybinding_active_ = false;
      }
    }
  } else {
    return_signal = processKeyEvents(key_events);
  }

  renderKeyboardConfigScreen();

  return return_signal;
}

KeyBindings KeyboardConfigProcessor::getKeyBindings() const {
  return key_bindings_;
}

void KeyboardConfigProcessor::setKeyBindings(const KeyBindings& key_bindings) {
  key_bindings_ = key_bindings;
}


}  // namespace nestris_x86
