#include "input_devices/sdl_gamepad.hpp"

#include <SDL.h>
#include <iso646.h>

#include <cmath>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "SDL_joystick.h"
#include "utils/logging.hpp"

namespace nestris_x86 {

namespace {

std::map<std::string, int> getReverseLookup(const std::map<int, std::string>& code_to_name) {
  std::map<std::string, int> name_to_code{};
  for (const auto& [id, name] : code_to_name) {
    name_to_code[name] = id;
  }
  return name_to_code;
}

class KeyCodeNameMap {
 public:
  KeyCodeNameMap() : name_to_code_{getReverseLookup(code_to_name_)} {}

  int nameToCode(const std::string& name) const { return name_to_code_.at(name); }

  std::string codeToName(const int code) const { return code_to_name_.at(code); }

  const std::map<std::string, int>& getNameToCodeMap() const { return name_to_code_; }
  const std::map<int, std::string>& getCodeToNameMap() const { return code_to_name_; }

  struct NewNameCode {
    std::string name{};
    int code{};
  };

  NewNameCode addEntry(const std::string& name_prefix) {
    const auto new_code = getUniqueCode();
    const auto new_name = getUniqueName(name_prefix);
    code_to_name_[new_code] = new_name;
    name_to_code_[new_name] = new_code;
    return {new_name, new_code};
  }

 private:
  int getUniqueCode() {
    int new_code = -1;
    while (code_to_name_.count(++new_code) != 0)
      ;
    return new_code;
  }

  std::string getUniqueName(const std::string& name_prefix) {
    int counter{};
    std::string new_name{};
    do {
      new_name = name_prefix + std::to_string(counter++);
    } while (name_to_code_.count(new_name) != 0);
    return new_name;
  }

  std::map<int, std::string> code_to_name_{
      {-1, "NONE"},    //
      {0, "FACE_D"},   //
      {1, "FACE_R"},   //
      {2, "FACE_L"},   //
      {3, "FACE_U"},   //
      {4, "L1"},       //
      {5, "R1"},       //
      {6, "START"},    //
      {7, "L3"},       //
      {8, "R3"},       //
      {10, "DPAD_L"},  //
      {11, "DPAD_R"},  //
      {12, "DPAD_U"},  //
      {13, "DPAD_D"}   //
  };
  std::map<std::string, int> name_to_code_;
};

}  // namespace

class SdlGamePad::Impl {
  using SdlKeyCode = int;

 public:
  Impl() : key_code_name_map_{} {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
      throw std::runtime_error("Couldn't initialize SDL: " + std::string(SDL_GetError()));
    }

    SDL_JoystickEventState(SDL_ENABLE);
    joystick_ = SDL_JoystickOpen(0);

    for (const auto& [code, name] : key_code_name_map_.getCodeToNameMap()) {
      button_states_[code] = 0;
    }
  }

  ~Impl() {}

  bool getKeyState(const KeyCode key_code) {
    pollAndUpdateInteralState();
    return button_states_.at(key_code);
  }

  InputInterface::KeyCode getPressedKey() {
    pollAndUpdateInteralState();
    for (const auto& [key_code, pressed] : button_states_) {
      if (pressed) {
        return key_code;
      }
    }
    return getNullKey();
  }

  std::string keyCodeToStr(const KeyCode key_code) const {
    if (not key_code_name_map_.getCodeToNameMap().count(key_code)) {
      LOG_ERROR("Unknown key code `" << key_code << "`.");
      return std::to_string(key_code);
    }
    return key_code_name_map_.codeToName(key_code);
  }

  InputInterface::KeyCode lookupKeyCode(const std::string& key_name) const {
    if (not key_code_name_map_.getNameToCodeMap().count(key_name)) {
      LOG_ERROR("No key code found for name `" << key_name << "`.");
      return getNullKey();
    }
    return key_code_name_map_.getNameToCodeMap().at(key_name);
  }

  InputInterface::KeyCode getNullKey() const { return -1; }

  void registerAxisAsButton(const int axis_number, const double axis_at_rest,
                            const double axis_pressed) {
    const auto new_button = key_code_name_map_.addEntry("AXISB");
    button_states_[new_button.code] = false;
    axis_triggers_.push_back({axis_number, axis_at_rest, axis_pressed, new_button.code});
  }

  std::vector<InputInterface::RegisteredAxisMovement> getRegisteredAxes() const {
    std::vector<InputInterface::RegisteredAxisMovement> registered_axes;
    for (const auto& axis_trigger : axis_triggers_) {
      registered_axes.emplace_back(InputInterface::RegisteredAxisMovement{
          axis_trigger.axis_number, axis_trigger.axis_at_rest, axis_trigger.axis_pressed});
    }
    return registered_axes;
  }

 private:
  struct AxisMovementTrigger {
    int axis_number;
    double axis_at_rest;
    double axis_pressed;
    int key_code;
  };

  void processAxisTriggers() {
    for (const auto& trigger : axis_triggers_) {
      if (axis_states_.count(trigger.axis_number) == 0) {
        continue;
      }
      const auto& axis_position = axis_states_.at(trigger.axis_number);
      button_states_[trigger.key_code] = (std::abs(trigger.axis_pressed - axis_position) <
                                          std::abs(trigger.axis_at_rest - axis_position));
    }
  }

  void pollAndUpdateInteralState() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
          button_states_[event.jbutton.button] = event.jbutton.state;
          break;

        case SDL_JOYAXISMOTION:
          axis_states_[event.jaxis.axis] = event.jaxis.value;
          break;

        case SDL_JOYHATMOTION: {
          const auto& hat = event.jhat.value;
          button_states_[key_code_name_map_.nameToCode("DPAD_U")] = bool(hat & SDL_HAT_UP);
          button_states_[key_code_name_map_.nameToCode("DPAD_D")] = bool(hat & SDL_HAT_DOWN);
          button_states_[key_code_name_map_.nameToCode("DPAD_R")] = bool(hat & SDL_HAT_RIGHT);
          button_states_[key_code_name_map_.nameToCode("DPAD_L")] = bool(hat & SDL_HAT_LEFT);
          break;
        }

        case SDL_QUIT:
          /* Set whatever flags are necessary to */
          /* end the main game loop here */
          break;
      }
    }
    processAxisTriggers();
  }
  KeyCodeNameMap key_code_name_map_;
  SDL_Joystick* joystick_;
  std::map<SdlKeyCode, bool> button_states_;
  std::map<int, double> axis_states_;
  std::vector<AxisMovementTrigger> axis_triggers_;
};  // namespace nestris_x86

SdlGamePad::SdlGamePad() : pimpl_{std::make_unique<SdlGamePad::Impl>()} {}

SdlGamePad::~SdlGamePad() = default;

bool SdlGamePad::getKeyState(const KeyCode key_code) {
  return pimpl_->getKeyState(key_code);
}

InputInterface::KeyCode SdlGamePad::getPressedKey() {
  return pimpl_->getPressedKey();
}

std::string SdlGamePad::keyCodeToStr(const KeyCode key_code) const {
  return pimpl_->keyCodeToStr(key_code);
}

InputInterface::KeyCode SdlGamePad::lookupKeyCode(const std::string& key_name) const {
  return pimpl_->lookupKeyCode(key_name);
}

InputInterface::KeyCode SdlGamePad::getNullKey() const {
  return pimpl_->getNullKey();
}

void SdlGamePad::registerAxisAsButton(const int axis_number, const double axis_at_rest,
                                      const double axis_pressed) {
  pimpl_->registerAxisAsButton(axis_number, axis_at_rest, axis_pressed);
}

std::vector<InputInterface::RegisteredAxisMovement> SdlGamePad::getRegisteredAxes() const {
  return pimpl_->getRegisteredAxes();
}

}  // namespace nestris_x86
