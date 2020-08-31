cmake_minimum_required(VERSION 2.6)
project(tetris_clone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

INCLUDE_DIRECTORIES(olc)
INCLUDE_DIRECTORIES(include)

FIND_PACKAGE(PNG REQUIRED)
FIND_PACKAGE(SDL_mixer REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR} ${SDL_MIXER_INCLUDE_DIRS})

# LINUX
# sudo apt install libsdl-mixer1.2-dev libsdl1.2-dev

if(UNIX AND NOT APPLE)
    MESSAGE("Building for linux...")
    SET(OLC_LIBS
            X11
            pthread
            PNG::PNG
            stdc++fs
            ${OPENGL_LIBRARIES}
            ${SDL_MIXER_LIBRARIES}
            ${SDL_LIBRARY}
        )
endif()

# MacOS
# brew install libpng sdl_mixer

if(APPLE)
    MESSAGE("Building for MacOS...")

    # TODO:: This probably needs to go BEFORE FIND_PACKAGE(SDL...)
    list(INSERT CMAKE_FRAMEWORK_PATH 0 /System/Library/Frameworks)

    FIND_PACKAGE(OpenGL REQUIRED)
    FIND_PACKAGE(GLUT REQUIRED)

    SET(OLC_LIBS
            GLUT::GLUT
            PNG::PNG
            ${OPENGL_LIBRARIES}
            ${SDL_MIXER_LIBRARY}
            ${SDL_LIBRARY}
        )
endif()

# end of OS specific

add_executable(tetris src/sound.cpp src/tetris.cpp src/main.cpp)
target_link_libraries(tetris ${OLC_LIBS})

