#define OLC_PGE_APPLICATION
#include <iostream>

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
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override { return true; }
};

int main() {
  Example demo;
  demo.Construct(10, 10, 1, 1);

  olc::Sprite test_sprite("assets/images/are-on.png");

  std::cout << test_sprite.height << std::endl;
  std::cout << test_sprite.width << std::endl;
  const auto *p = test_sprite.GetData();
  for (int i = 0; i < test_sprite.height * test_sprite.width; ++i) {
    std::cout << p[i].n << " ";
  }
  std::cout << std::endl;

  return 0;
}
