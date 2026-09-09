#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
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


FILE* rom;
Chip8 app;

void fetch(){
  app.opcode = app.memory[app.PC] << 8 | app.memory[app.PC + 1];
  app.PC += 2;
}
void decode(){
  uint8_t x, y;
  uint16_t n;
  switch (app.opcode & 0xF000) {
    case 0x0000:
      if (app.opcode == 0x00E0) {
        memset(app.display, false, 64*32);
        app.render_flag = true;
      }
    break;
    case 0x1000:
      app.PC = app.opcode & 0x0FFF;
    break;
    case 0x2000:

    break;
    case 0x3000:

    break;
    case 0x4000:

    break;
    case 0x5000:

    break;
    case 0x6000:
      app.V[(app.opcode & 0x0F00) >> 8] = app.opcode & 0x00FF;
    break;
    case 0x7000:
      app.V[(app.opcode & 0x0F00) >> 8] += app.opcode & 0x00FF;
    break;
    case 0x8000:

    break;
    case 0x9000:

    break;
    case 0xA000:
      app.IR = app.opcode & 0x0FFF;
    break;
    case 0xB000:

    break;
    case 0xC000:

    break;
    case 0xD000:
      uint8_t byte;
      uint8_t x0 = app.V[(app.opcode & 0x0F00) >> 8] & 63;
      y = app.V[(app.opcode & 0x00F0) >> 4] & 31;
      n = app.opcode & 0x000F;
      app.V[0xF] = 0;
      for (int i = 0; i < n; i++) {
        x = x0;
        byte = app.memory[app.IR+i];
        for (int bit = 7; bit > -1; bit--){
          if ((byte >> bit) & 1){
            if (app.display[y*64+x]){
              app.V[0xF]=1;
            }
            app.display[y*64+x] ^= 1;
          }
          x++;
          if (x >= 64) {
            break;
          }
        }
        y++;
        if (y >= 32) {
          break;
        }
      }
      app.render_flag = true;
    break;
    case 0xE000:

    break;
    case 0xF000:

    break;
  }
}
void Chip8_Start(const char* path_to_rom){
  app.PC = 0x200;
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
}
void Chip8_Step(){
    app.render_flag = false;
    fetch();
    decode();
}
