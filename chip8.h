#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t V[16]; // Registers
  uint16_t IR;   // Index Register
  uint16_t PC;   // Program Pointer
  uint8_t SP;    // Stack Pointer
  uint16_t stack[16];
  uint8_t key[16];
  uint8_t timer;
  uint8_t sound_timer;
  uint8_t memory[4096];
  uint16_t opcode;
  bool display[64*32];
  bool render_flag;
}Chip8;
extern Chip8 app;
void Chip8_Start(const char* rom);
void Chip8_Step();
bool Chip8_Check_Render();
