#pragma once
#include <stdint.h>

extern volatile int32_t PALETTE_TIMER;
extern volatile uint8_t *NEW_PALETTE;

extern "C" void set_palette(uint8_t *pal_file);
extern "C" void set_palette_2(uint8_t *pal_ptr, int offset, int num_entries);
void palette_animation();
extern "C" void swap_palette_with_animation(uint8_t *palette_file_ptr, int time);
extern "C" void wait_palette_animation(int target_time);
extern "C" uint8_t *get_RGB_palette_ptr();