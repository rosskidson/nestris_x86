#include "olc_keyboard.hpp"

namespace tetris_clone {

// copy pasted from olc source code and formatted to strings.
// clang-format off
const std::array<std::string, 85> OlcKeyboard::key_names{
		"NONE",
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		"K0", "K1", "K2", "K3", "K4", "K5", "K6", "K7", "K8", "K9",
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
		"UP", "DOWN", "LEFT", "RIGHT",
		"SPACE","TAB","SHIFT","CTRL","INS","DEL","HOME","END","PGUP","PGDN",
		"BACK","ESCAPE","RETURN","ENTER","PAUSE","SCROLL",
		"NP0","NP1","NP2","NP3","NP4","NP5","NP6","NP7","NP8","NP9",
		"NP_MUL","NP_DIV","NP_ADD","NP_SUB","NP_DECIMAL","PERIOD"
};

std::map<std::string, int> getLookupTable(const std::array<std::string, 85> & key_names) {
  std::map<std::string, int> name_code_lookup{};
  for (int i = 0; i < key_names.size(); ++i) {
    name_code_lookup[key_names[i]] = i;
  }
  return name_code_lookup;
}

const std::map<std::string, int> OlcKeyboard::name_code_lookup = getLookupTable(OlcKeyboard::key_names);

// clang-format on

OlcKeyboard::OlcKeyboard(olc::PixelGameEngine& pixel_game_engine_ref)
    : pixel_game_engine_ref_(pixel_game_engine_ref) {}

bool OlcKeyboard::getKeyState(const KeyCode key_code) const {
  return pixel_game_engine_ref_.GetKey(keyCodeToOlc(key_code)).bHeld;
}

InputInterface::KeyCode OlcKeyboard::getPressedKey() const {
  for (int i = 1; i < num_keys; ++i) {
    if (getKeyState(i)) {
      return i;
    }
  }
  return 0;
}

std::string OlcKeyboard::keyCodeToStr(const KeyCode key_code) const {
  return key_names.at(key_code);
}

InputInterface::KeyCode OlcKeyboard::lookupKeyCode(const std::string& key_name) const {
  return name_code_lookup.at(key_name);
}

InputInterface::KeyCode OlcKeyboard::olcToKeyCode(const olc::Key& key) {
  return static_cast<olc::Key>(key);
}

olc::Key OlcKeyboard::keyCodeToOlc(const KeyCode key_code) {
  return static_cast<olc::Key>(key_code);
}

}  // namespace tetris_clone
