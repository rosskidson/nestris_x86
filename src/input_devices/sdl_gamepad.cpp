#include "input_devices/sdl_gamepad.hpp"

#include <SDL.h>
#include <iso646.h>

#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "SDL_joystick.h"
#include "utils/logging.hpp"

namespace nestris_x86 {

namespace {

const std::map<int, std::string> CODE_TO_NAME{
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
// clang-format on
constexpr int OLC_NUMBER_OF_KEYS = 18;

std::map<std::string, int> getReverseLookup(const std::map<int, std::string>& code_to_name) {
  std::map<std::string, int> name_to_code{};
  for (const auto& [id, name] : code_to_name) {
    name_to_code[name] = id;
  }
  return name_to_code;
}

const std::map<std::string, int> NAME_TO_CODE = getReverseLookup(CODE_TO_NAME);

}  // namespace

class SdlGamePad::Impl {
  using SdlKeyCode = int;

 public:
  Impl() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
      throw std::runtime_error("Couldn't initialize SDL: " + std::string(SDL_GetError()));
    }

    SDL_JoystickEventState(SDL_ENABLE);
    joystick_ = SDL_JoystickOpen(0);

    for (const auto& [code, name] : CODE_TO_NAME) {
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
    if (not CODE_TO_NAME.count(key_code)) {
      LOG_ERROR("Unknown key code `" << key_code << "`.");
      return std::to_string(key_code);
    }
    return CODE_TO_NAME.at(key_code);
  }

  InputInterface::KeyCode lookupKeyCode(const std::string& key_name) const {
    if (not NAME_TO_CODE.count(key_name)) {
      LOG_ERROR("No key code found for name `" << key_name << "`.");
      return getNullKey();
    }
    return NAME_TO_CODE.at(key_name);
  }

  InputInterface::KeyCode getNullKey() const { return -1; }

 private:
  void pollAndUpdateInteralState() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
          button_states_[event.jbutton.button] = event.jbutton.state;
          break;

          // case SDL_JOYAXISMOTION: [> Handle Joystick Motion <]
          //  std::cout << "Axis: " << (int)event.jaxis.axis << " " << event.jaxis.value <<
          //  std::endl; break;

        case SDL_JOYHATMOTION: {
          const auto& hat = event.jhat.value;
          button_states_[NAME_TO_CODE.at("DPAD_U")] = bool(hat & SDL_HAT_UP);
          button_states_[NAME_TO_CODE.at("DPAD_D")] = bool(hat & SDL_HAT_DOWN);
          button_states_[NAME_TO_CODE.at("DPAD_R")] = bool(hat & SDL_HAT_RIGHT);
          button_states_[NAME_TO_CODE.at("DPAD_L")] = bool(hat & SDL_HAT_LEFT);
          break;
        }

        case SDL_QUIT:
          /* Set whatever flags are necessary to */
          /* end the main game loop here */
          break;
      }
    }
  }
  SDL_Joystick* joystick_;
  std::map<SdlKeyCode, bool> button_states_;
};

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

}  // namespace nestris_x86
