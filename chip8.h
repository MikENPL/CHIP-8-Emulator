#pragma once
#include <stdbool.h>

#define WIDTH 64
#define HEIGHT 32

void Chip8_Start(const char* rom);
void Chip8_Step();
bool Chip8_Get_Render_Flag();
bool* Chip8_Get_Display();
