#define OLC_PGE_APPLICATION
#include "tetris.hpp"

int main()
{
  tetris_clone::TetrisClone tetris;
	if (tetris.Construct(256, 225, 4, 4))
  {
		tetris.Start();
  }
	return 0;
}
