cmake_minimum_required(VERSION 2.6)
project(tetris_clone)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
        )
endif()

# MacOS
# brew install libpng sdl_mixer

if(APPLE)
    MESSAGE("Building for MacOS...")

    # I would have thought that this needs to go BEFORE FIND_PACKAGE(SDL...)
    list(INSERT CMAKE_FRAMEWORK_PATH 0 /System/Library/Frameworks)

    FIND_PACKAGE(OpenGL REQUIRED)
    FIND_PACKAGE(GLUT REQUIRED)

    SET(OLC_LIBS
            GLUT::GLUT
            PNG::PNG
            ${OPENGL_LIBRARIES}
        )

    SET(GAMEPAD_CPPS "src/frame_processors/gamepad_config_processor_mac.cpp")
else()
    SET(GAMEPAD_CPPS "src/frame_processors/gamepad_config_processor.cpp src/olc_gamepad.cpp")
endif()

# end of OS specific


SET(TETRIS_LIBS
  ${SDL_MIXER_LIBRARIES}
  ${SDL_LIBRARY}
)

add_library(olc olc/olcPixelGameEngine.cpp)
target_link_libraries(olc ${OLC_LIBS})

add_library(assets_lib src/assets_cpp/images.cpp)

add_executable(tetris
        src/assets.cpp
        src/drawing_utils.cpp
        src/game_logic.cpp
        src/main.cpp
        src/olc_drawer.cpp
        src/olc_keyboard.cpp
        #src/olc_gamepad.cpp
        src/olc_sprite_encoder.cpp
        src/game_renderer.cpp
        src/frame_processors/game_processor.cpp
        src/frame_processors/level_screen_processor.cpp
        src/frame_processors/option_screen_processor.cpp
        src/frame_processors/keyboard_config_processor.cpp
        #src/frame_processors/gamepad_config_processor.cpp
        src/sound.cpp
        src/tetris.cpp
        ${GAMEPAD_CPPS}
        )


target_link_libraries(tetris olc assets_lib ${TETRIS_LIBS})


#add_executable(olc_sprite_gen src/olc_sprite_gen.cpp src/olc_sprite_encoder.cpp)
#target_link_libraries(olc_sprite_gen ${OLC_LIBS})

#add_executable(testbed src/boilerplate.cpp)
#target_link_libraries(testbed olc ${OLC_LIBS})
