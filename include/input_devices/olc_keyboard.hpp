#pragma once

#include <map>
#include <vector>

#include "input_interface.hpp"
#include "olcPixelGameEngine.h"

namespace nestris_x86 {

class OlcKeyboard : public InputInterface {
 public:
  OlcKeyboard(olc::PixelGameEngine& pixel_game_engine_ref);

  bool getKeyState(const KeyCode key_code) override;

  KeyCode getPressedKey() override;

  std::string keyCodeToStr(const KeyCode key_code) const override;
  KeyCode lookupKeyCode(const std::string& key_name) const override;

  KeyCode getNullKey() const override;

  void registerAxisAsButton(const int axis_number, const double axis_at_rest,
                            const double axis_pressed) override;

  std::vector<RegisteredAxisMovement> getRegisteredAxes() const override;

 private:
  static KeyCode olcToKeyCode(const olc::Key& key);
  static olc::Key keyCodeToOlc(const KeyCode key_code);

  olc::PixelGameEngine& pixel_game_engine_ref_;
  static constexpr int num_keys{85};
  const static std::map<std::string, int> name_code_lookup;
  const static std::array<std::string, 85> key_names;
};

}  // namespace nestris_x86
