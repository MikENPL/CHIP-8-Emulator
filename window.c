#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_scancode.h>
#include <stdbool.h>
#include <stdio.h>
#include "chip8.h"

#define SCALE 10
#define BACKGROUND_COLOR 0, 0, 0, 255
#define PIXEL_COLOR 255, 255, 255, 255

int keymap[16] = {
  SDL_SCANCODE_X,
  SDL_SCANCODE_1,
  SDL_SCANCODE_2,
  SDL_SCANCODE_3,
  SDL_SCANCODE_Q,
  SDL_SCANCODE_W,
  SDL_SCANCODE_E,
  SDL_SCANCODE_A,
  SDL_SCANCODE_S,
  SDL_SCANCODE_D,
  SDL_SCANCODE_Z,
  SDL_SCANCODE_C,
  SDL_SCANCODE_4,
  SDL_SCANCODE_R,
  SDL_SCANCODE_F,
  SDL_SCANCODE_V,
};

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
    switch (event.type) {
      case SDL_EVENT_QUIT:
        *running = false;
      break;
      case SDL_EVENT_KEY_DOWN:
        for (int i = 0; i < 16; i++) {
          if (event.key.scancode == keymap[i]) {
            Chip8_Set_Key(i, true);
          }
        }
      break;
      case SDL_EVENT_KEY_UP:
        for (int i = 0; i < 16; i++) {
          if (event.key.scancode == keymap[i]) {
            Chip8_Set_Key(i, false);
          }
        }
      break;
    }
  }
}
void Window_Start(){
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer("CHIP-8", WIDTH*SCALE, HEIGHT*SCALE, 0, &window, &renderer);
}
