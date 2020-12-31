#include "nestris_x86.hpp"

int main(const int argc, const char** argv)
{
  nestris_x86::NestrisX86 nestetris{};
	if (nestetris.Construct(256, 225, 4, 4))
  {
    nestetris.Start();
  }
	return 0;
}
