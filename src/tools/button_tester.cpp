#include <chrono>
#include <iostream>
#include <thread>

#include "olcPGEX_Gamepad.h"
#include "olcPixelGameEngine.h"

class Example : public olc::PixelGameEngine {
 public:
  Example() {
    // Name you application
    sAppName = "Example";
  }

 public:
  bool OnUserCreate() override {
    // Called once at the start, so create things here
    olc::GamePad::init();
    gamepads = olc::GamePad::getGamepads();
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    for (int x = 0; x < ScreenWidth(); x++)
      for (int y = 0; y < ScreenHeight(); y++)
        Draw(x, y, olc::Pixel(rand() % 255, rand() % 255, rand() % 255));
    if (!player1.valid) {
      player1 = detectGamePad();
      std::cout << "Awaiting init... " << std::endl;
    } else {
      player1.poll();
    }
    std::cout << player1.getButton(olc::GPButtons::FACE_U).bHeld << std::endl;
    std::cout << player1.getButton(olc::GPButtons::FACE_D).bHeld << std::endl;
    std::cout << player1.getButton(olc::GPButtons::FACE_L).bHeld << std::endl;
    std::cout << player1.getButton(olc::GPButtons::FACE_R).bHeld << std::endl;
    std::cout << player1.getButton(olc::GPButtons::DPAD_U).bHeld << std::endl;
    std::cout << player1.getButton(olc::GPButtons::DPAD_D).bHeld << std::endl;
    std::cout << player1.getButton(olc::GPButtons::DPAD_L).bHeld << std::endl;
    std::cout << player1.getButton(olc::GPButtons::DPAD_R).bHeld << std::endl;
    std::cout << "#####################" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return true;
  }

 private:
  olc::GamePad detectGamePad() {
    for (auto& gamepad : gamepads) {
      gamepad.poll();
      for (int i = 0; i < 18; ++i) {
        if (i == 12 || i == 13) {
          continue;
        }
        if (gamepad.getButton(static_cast<olc::GPButtons>(i)).bHeld) {
          return gamepad;
        }
      }
    }
    return {};
  }

  olc::GamePad player1;
  std::vector<olc::GamePad> gamepads;
};

int main() {
  Example demo;
  demo.Construct(100, 100, 1, 1);
  demo.Start();

  return 0;
}
