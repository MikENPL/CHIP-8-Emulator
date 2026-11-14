#include <SDL3/SDL_timer.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "chip8.h"

uint8_t font[]= {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
0x20, 0x60, 0x20, 0x20, 0x70, // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
0x90, 0x90, 0xF0, 0x10, 0x10, // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
0xF0, 0x10, 0x20, 0x40, 0x40, // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
0xF0, 0x90, 0xF0, 0x90, 0x90, // A
0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
0xF0, 0x80, 0x80, 0x80, 0xF0, // C
0xE0, 0x90, 0x90, 0x90, 0xE0, // D
0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
0xF0, 0x80, 0xF0, 0x80, 0x80  // F

};

typedef struct {
  uint8_t V[16]; // Registers
  uint16_t IR;   // Index Register
  uint16_t PC;   // Program Pointer
  int SP;        // Stack Pointer
  uint16_t stack[16];
  bool key[16];
  uint8_t timer;
  uint8_t sound_timer;
  uint8_t memory[4096];
  uint16_t opcode;
  bool display[WIDTH*HEIGHT];
  bool render_flag;
}Chip8;

FILE* rom;
Chip8 app;
uint64_t last_time;

void fetch(){
  app.opcode = app.memory[app.PC] << 8 | app.memory[app.PC + 1];
  app.PC += 2;
}
void decode(){
  uint8_t x, y;
  uint16_t n;
  switch (app.opcode & 0xF000) {
    case 0x0000:
      if (app.opcode == 0x00E0){
        memset(app.display, false, WIDTH*HEIGHT);
        app.render_flag = true;
      }else { 
        app.SP--;
        app.PC = app.stack[app.SP]; //GDB
      }
    break;
    case 0x1000:
      app.PC = app.opcode & 0x0FFF;
    break;
    case 0x2000:
      app.stack[app.SP] = app.PC;
      app.SP++;
      app.PC = app.opcode & 0x0FFF;
    break;
    case 0x3000:
      if (app.V[(app.opcode & 0x0F00) >> 8] == (app.opcode & 0x00FF)) {
        app.PC += 2;
      }
    break;
    case 0x4000:
      if (app.V[(app.opcode & 0x0F00) >> 8] != (app.opcode & 0x00FF)) {
        app.PC += 2;
      }
    break;
    case 0x5000:
      if (app.V[(app.opcode & 0x0F00) >> 8] == app.V[(app.opcode & 0x00F0) >> 4]) {
        app.PC += 2;
      }
    break;
    case 0x6000:
      app.V[(app.opcode & 0x0F00) >> 8] = app.opcode & 0x00FF;
    break;
    case 0x7000:
      app.V[(app.opcode & 0x0F00) >> 8] += app.opcode & 0x00FF;
    break;
    case 0x8000:
      switch (app.opcode & 0x000F) {
        case 0x0000:
          app.V[(app.opcode & 0x0F00) >> 8] = app.V[(app.opcode & 0x00F0) >> 4];
        break;
        case 0x0001:
          app.V[(app.opcode & 0x0F00) >> 8] |= app.V[(app.opcode & 0x00F0) >> 4];
        break;
        case 0x0002:
          app.V[(app.opcode & 0x0F00) >> 8] &= app.V[(app.opcode & 0x00F0) >> 4];
        break;
        case 0x0003:
          app.V[(app.opcode & 0x0F00) >> 8] ^= app.V[(app.opcode & 0x00F0) >> 4];
        break;
        case 0x0004:
          app.V[(app.opcode & 0x0F00) >> 8] += app.V[(app.opcode & 0x00F0) >> 4];
          if (app.V[(app.opcode & 0x00F0) >> 4] > (0xFF - app.V[(app.opcode & 0x0F00) >> 8])) {
            app.V[0xF] = 1;
          }else {
            app.V[0xF] = 0;
          }
        break;
        case 0x0005:
          if (app.V[(app.opcode & 0x00F0) >> 4] > app.V[(app.opcode & 0x0F00) >> 8]) {
            app.V[0xF] = 0;
          } else {
            app.V[0xF] = 1;
          }
          app.V[(app.opcode & 0x0F00) >> 8] -= app.V[(app.opcode & 0x00F0) >> 4];
        break;
        case 0x0006:
          app.V[0xF] = app.V[(app.opcode & 0x0F00) >> 8] & 0x1;
          app.V[(app.opcode & 0x0F00) >> 8] >>= 1;
        break;
        case 0x0007:
        if (app.V[(app.opcode & 0x0F00) >> 8] > app.V[(app.opcode & 0x00F0) >> 4]) {
          app.V[0xF] = 0;
        } else {
          app.V[0xF] = 1;
        }
        app.V[(app.opcode & 0x0F00) >> 8] = app.V[(app.opcode & 0x00F0) >> 4] - app.V[(app.opcode & 0x0F00) >> 8] ;
        break;
        case 0x000E:
          app.V[0xF] = app.V[(app.opcode & 0x0F00) >> 8] >> 7;
          app.V[(app.opcode & 0x0F00) >> 8] <<= 1;
        break;
      }
    break;
    case 0x9000:
      if (app.V[(app.opcode & 0x0F00) >> 8] != app.V[(app.opcode & 0x00F0) >> 4]) {
        app.PC += 2;
      }
    break;
    case 0xA000:
      app.IR = app.opcode & 0x0FFF;
    break;
    case 0xB000:
      app.PC = app.V[0] + (app.opcode & 0x0FFF);
    break;
    case 0xC000:
      app.V[(app.opcode & 0x0F00) >> 8] = (rand() & (0xFF + 1)) & (app.opcode & 0x00FF);
    break;
    case 0xD000: //TOCHECK
      uint8_t byte;
      uint8_t x0 = app.V[(app.opcode & 0x0F00) >> 8] % WIDTH;
      y = app.V[(app.opcode & 0x00F0) >> 4] % HEIGHT;
      n = app.opcode & 0x000F;
      app.V[0xF] = 0;
      for (int i = 0; i < n; i++) {
        x = x0;
        byte = app.memory[app.IR+i];
        for (int bit = 7; bit > -1; bit--){
          if ((byte >> bit) & 1){
            if (app.display[y*WIDTH+x]){
              app.V[0xF]=1;
            }
            app.display[y*WIDTH+x] ^= 1;
          }
          x++;
          if (x >= WIDTH) {
            break;
          }
        }
        y++;
        if (y >= HEIGHT) {
          break;
        }
      }
      app.render_flag = true;
    break;
    case 0xE000: //TOCHECK
      switch (app.opcode & 0x00FF){
        case 0x009E:
          if (app.key[app.V[(app.opcode & 0x0F00) >> 8]] != 0)
          {
            app.PC += 2;
          }
        break;
        case 0x00A1:
          if (app.key[app.V[(app.opcode & 0x0F00) >> 8]] == 0)
          {
            app.PC += 2;
          }
        break;
      }
    break;
    case 0xF000:
      switch (app.opcode & 0x00FF) {
        case 0x0007:
          app.V[(app.opcode & 0x0F00) >> 8] = app.timer;
        break;
        case 0x000A: //TOCHECK
          bool key_pressed = false;
          app.PC -= 2;
          for (int i = 0; i < 16; i++) {
            if (app.key[i]) {
              app.V[(app.opcode & 0x0F00) >> 8] = i;
              key_pressed = true;
              break;
            }
          }
          if (!key_pressed) {
            return;
          }
          app.PC += 2;
        break;
        case 0x0015:
          app.timer = app.V[(app.opcode & 0x0F00) >> 8];
        break;
        case 0x0018:
          app.sound_timer = app.V[(app.opcode & 0x0F00) >> 8];
        break;
        case 0x001E:
          if (app.IR + app.V[(app.opcode & 0x0F00) >> 8] > 0xFFF)
          {
            app.V[0xF] = 1;
          }else{
            app.V[0xF] = 0;
          }
          app.IR += app.V[(app.opcode & 0x0F00) >> 8];
        break;
        case 0x0029:
          app.IR = app.memory[app.V[(app.opcode & 0x0F00) >> 8]* 0x5 + 0x50];
        break;
        case 0x0033:
          app.memory[app.IR] = app.V[(app.opcode & 0x0F00) >> 8]/100;
          app.memory[app.IR + 1] = (app.V[(app.opcode & 0x0F00) >> 8]/10)%10;
          app.memory[app.IR + 2] =  app.V[(app.opcode & 0x0F00) >> 8]%10;
        break;
        case 0x0055:
          for (int i = 0; i <= ((app.opcode & 0x0F00) >> 8); i++) {
            app.memory[app.IR + i] = app.V[i];
          }
        break;
        case 0x0065:
          for (int i = 0; i <= ((app.opcode & 0x0F00) >> 8); i++) {
            app.V[i] = app.memory[app.IR + i] ;
          }
          app.IR += ((app.opcode & 0x0F00) >> 8) + 1;
        break;
      }

    break;
  }
}
void Chip8_Start(const char* path_to_rom){
  app.PC = 0x200;
  app.SP = 0;
  srand(time(NULL));
  last_time = SDL_GetPerformanceCounter();
  rom = fopen(path_to_rom, "rb");
  fseek(rom, 0L, SEEK_END);
  size_t rom_size = ftell(rom);
  rewind(rom);
  if (rom_size > 0xFFF - 0x200) {
    printf("Error: ROM too large.\n");
    return;
  }
  fread(&app.memory[0x200], 1, rom_size, rom);
  memcpy(&app.memory[0x50],font, 80);
  fclose(rom);
}
bool* Chip8_Get_Display(){
  return app.display;
}
bool Chip8_Get_Render_Flag(){
  return app.render_flag;
}
void Chip8_Set_Key(int index, bool value){
  app.key[index] = value;
}
void update_timers(){
  uint64_t current_time = SDL_GetPerformanceCounter();
  uint64_t interval = SDL_GetPerformanceFrequency()/60;
  while (current_time - last_time >= interval) {
    if (app.timer > 0) {
      app.timer--;
    }
    if (app.sound_timer > 0) {
      app.sound_timer--;
    }
    last_time += interval;
  }
}
void Chip8_Step(){
    app.render_flag = false;
    fetch();
    //printf("Opcode: %X\n", app.opcode);
    decode();
    update_timers();
}
