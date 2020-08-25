cmake_minimum_required(VERSION 2.6)
project(tetris_clone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

INCLUDE_DIRECTORIES(olc)
INCLUDE_DIRECTORIES(include)

# LINUX
#SET(OLC_LIBS X11 GL pthread png stdc++fs asound)

# MacOS

# brew install libpng sdl_mixer

list(INSERT CMAKE_FRAMEWORK_PATH 0 /System/Library/Frameworks)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLUT REQUIRED)
FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(SDL_mixer REQUIRED)
FIND_PACKAGE(SDL REQUIRED)

INCLUDE_DIRECTORIES(${SDL_MIXER_INCLUDE_DIRS})

#### SDL2 stuff (not neccessary, sdl 1 is fine.)
#FIND_PACKAGE(SDL2 REQUIRED)
#INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})

#set(SDL2_MIXER_INCLUDE_DIRS /usr/local/Cellar/sdl2_mixer/2.0.4/include)
#set(SDL2_MIXER_LINK_DIR /usr/local/Cellar/sdl2_mixer/2.0.4/lib/)
#set(SDL2_MIXER_LIBRARY -lSDL2_mixer)
#INCLUDE_DIRECTORIES(${SDL2_MIXER_INCLUDE_DIRS})
#LINK_DIRECTORIES(${SDL2_MIXER_LINK_DIR})
#MESSAGE(${SDL2_LIBRARIES})
###########################################

SET(OLC_LIBS GLUT::GLUT PNG::PNG ${OPENGL_LIBRARIES} ${SDL_MIXER_LIBRARY} ${SDL_LIBRARY})

# end of OS specific

add_executable(tetris src/tetris.cpp src/main.cpp)
target_link_libraries(tetris ${OLC_LIBS})


add_executable(sound_test src/sound_test.cpp)
#target_link_libraries(sound_test ${SDL2_MIXER_LIBRARY} SDL2::SDL2)
target_link_libraries(sound_test ${SDL_MIXER_LIBRARY} ${SDL_LIBRARY})
