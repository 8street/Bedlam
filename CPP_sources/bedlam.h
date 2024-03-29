#pragma once
#include <cstdint>
///////////////////////////////////////////////////
// Here function prototypes from bedlam.asm file //
// Here extern variables from bedlam.asm file    //
///////////////////////////////////////////////////

extern "C" {
int game_core1();
void level_clock();
void midi_callback();
void free_mem();
}

extern "C" volatile int32_t screen_x_pos;
extern "C" volatile int32_t screen_y_pos;
extern "C" volatile int32_t game_is_playing;
extern "C" volatile int32_t map_active;
extern "C" volatile int32_t dead_screen_scale;