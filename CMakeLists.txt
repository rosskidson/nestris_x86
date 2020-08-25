cmake_minimum_required(VERSION 2.6)
project(tetris_clone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

INCLUDE_DIRECTORIES(olc)
INCLUDE_DIRECTORIES(include)

# LINUX
#SET(OLC_LIBS X11 GL pthread png stdc++fs asound)

# MacOS

list(INSERT CMAKE_FRAMEWORK_PATH 0 /System/Library/Frameworks)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(PNG REQUIRED)

SET(OLC_LIBS GLUT::GLUT PNG::PNG ${OPENGL_LIBRARIES})

# end of OS specific

add_executable(tetris src/tetris.cpp src/main.cpp)
target_link_libraries(tetris ${OLC_LIBS})

