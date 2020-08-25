
//#include <SDL.h>
#include <SDL_mixer.h>

#include <iostream>
#include <unistd.h>

int main(const int argc, const char** argv) {
  // SDL_Init(SDL_INIT_AUDIO );

  // Set up the audio stream
  int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
  if (result < 0) {
    fprintf(stderr, "Unable to open audio: ");
    exit(-1);
  }

  result = Mix_AllocateChannels(4);
  if (result < 0) {
    fprintf(stderr, "Unable to allocate mixing channels: " );
    exit(-1);
  }

  // Load waveforms
  auto sample = Mix_LoadWAV("./assets/sounds/tetromino_move.wav");
  auto sample_a = Mix_LoadWAV("./assets/sounds/tetris.wav");
  if (sample == NULL) {
    fprintf(stderr, "Unable to load wave file\n");
  }

  constexpr int sleep = 0.02 * 10000000.0;
  int i = 1;
  while(1) {
    if(i++ % 8 == 0) {
      Mix_PlayChannel(-1, sample_a, 0);
    }
    Mix_PlayChannel(-1, sample, 0);
    usleep(sleep);
  }

  Mix_CloseAudio();
  return 0;
}
