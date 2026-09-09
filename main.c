//#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
//#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>
#include <stdio.h>
#include "chip8.h"

extern Chip8 app;

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: chip8 <rom>\n");
    return 1;
  }
  Chip8_Start(argv[1]);
  while (true) {
    Chip8_Step();
    if (app.render_flag) {
      for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
          if (app.display[64*y+x]) {
            printf("#");
          }else {
            printf(" ");
          }
        }
        printf("\n");
      }
    }
  }
}

