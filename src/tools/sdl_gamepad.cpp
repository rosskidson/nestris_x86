#include <SDL.h>
#include <SDL_image.h>
#include <signal.h>
#include <stdio.h>

#include <cmath>
#include <iostream>
#include <map>
#include <string>

void signal_callback_handler(int signum) {
  std::cout << "Caught signal " << signum << std::endl;
  // Terminate program
  exit(signum);
}

int main() {
  signal(SIGINT, signal_callback_handler);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  printf("%i joysticks were found.\n\n", SDL_NumJoysticks());
  printf("The names of the joysticks are:\n");

  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    printf("    %s\n", SDL_JoystickName(i));
  }

  SDL_Joystick* joystick;

  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);
  int number_of_buttons = SDL_JoystickNumButtons(joystick);
  std::cout << "Number of buttons " << number_of_buttons << std::endl;

  SDL_Event event;
  std::map<int, double> axis_states;

  /* Start main game loop here */
  while (1) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_JOYBUTTONUP:   /* Handle Joystick Button Presses */
        case SDL_JOYBUTTONDOWN: /* Handle Joystick Button Presses */
          std::cout << "Button " << (int)event.jbutton.button << ": " << (bool)event.jbutton.state
                    << std::endl;
          break;

        case SDL_JOYBALLMOTION: /* Handle Joyball Motion */
          std::cout << "'joy ball': " << (int)event.jball.ball << " " << (int)event.jball.which
                    << std::endl;
          break;

        case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
          axis_states[event.jaxis.axis] = event.jaxis.value;
          for (const auto& [axis, axis_val] : axis_states) {
            std::cout << "Axis[" << axis << "]: " << axis_val << std::endl;
          }
          break;

        case SDL_JOYHATMOTION: /* Handle Hat Motion */
          std::cout << "Hat: " << (int)event.jhat.hat << " " << (int)event.jhat.value << std::endl;
          break;

        case SDL_QUIT:
          /* Set whatever flags are necessary to */
          /* end the main game loop here */
          break;
      }
    }
  }
}
