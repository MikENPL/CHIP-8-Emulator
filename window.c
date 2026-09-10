#include <SDL3/SDL.h>
#include <stdbool.h>
#include "chip8.h"

#define SCALE 10
#define BACKGROUND_COLOR 0, 0, 0, 255
#define PIXEL_COLOR 255, 255, 255, 255
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;

void Window_Render_Display(){
  bool* display = Chip8_Get_Display();
  SDL_FRect pixel = {.w = SCALE, .h = SCALE};
  SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, PIXEL_COLOR);
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (display[WIDTH*y+x]) {
        pixel.x = x*SCALE;
        pixel.y = y*SCALE;
        SDL_RenderFillRect(renderer, &pixel);
      }
    }
  }
  SDL_RenderPresent(renderer);
}
void Window_Handle_Events(bool* running){
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      *running = false;
    }
  }
}
void Window_Start(){
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer("CHIP-8", WIDTH*SCALE, HEIGHT*SCALE, 0, &window, &renderer);
}
