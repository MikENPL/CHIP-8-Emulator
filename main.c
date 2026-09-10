#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "chip8.h"
#include "window.h"

bool running = true;

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: chip8 <rom>\n");
    return 1;
  }
  Chip8_Start(argv[1]);
  Window_Start();
  while (running) {
    Chip8_Step();
    if (Chip8_Get_Render_Flag()) {
      Window_Render_Display();
    }
    Window_Handle_Events(&running);
  }
  SDL_Quit();
  return 0;
}

