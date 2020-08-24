#define OLC_PGE_APPLICATION
#include "tetris.hpp"

int getStartLevel(const int argc, const char** argv){
  if(argc == 1) {
    return 0;
  } else {
    return std::stoi(argv[1]);
  }
}

int main(const int argc, const char** argv)
{
  tetris_clone::TetrisClone tetris(getStartLevel(argc, argv));
	if (tetris.Construct(256, 225, 4, 4))
  {
    tetris.Start();
  }
	return 0;
}
