cmake_minimum_required(VERSION 2.6)
project(tetris_clone)

INCLUDE_DIRECTORIES(../olcPixelGameEngine)
SET(OLC_LIBS X11 GL pthread png stdc++fs)

INCLUDE_DIRECTORIES(include)

add_executable(hello_world src/hello_world.cpp)
target_link_libraries(hello_world ${OLC_LIBS})

add_executable(tetris src/tetris.cpp src/main.cpp)
target_link_libraries(tetris ${OLC_LIBS})

