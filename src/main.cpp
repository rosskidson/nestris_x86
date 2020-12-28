#include "tetris.hpp"

int main(const int argc, const char** argv)
{
  nestris_x86::TetrisClone tetris{};
	if (tetris.Construct(256, 225, 4, 4))
  {
    tetris.Start();
  }
	return 0;
}
