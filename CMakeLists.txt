cmake_minimum_required(VERSION 2.6)
project(tetris_clone)

set(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(olc)
SET(OLC_LIBS X11 GL pthread png stdc++fs asound)

INCLUDE_DIRECTORIES(include)

add_executable(tetris src/tetris.cpp src/main.cpp)
target_link_libraries(tetris ${OLC_LIBS})

