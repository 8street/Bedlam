#pragma once

#include <stdint.h>

#include "sdl_cursor.h"

extern cursor WINDOW_CURSOR;

extern int16_t CURSOR_X1;
extern int16_t CURSOR_Y1;

extern uint16_t CURSOR_SURFACE_SIZE;

extern "C" volatile uint8_t UPDATE_CURSOR_BY_TIMER;
extern uint8_t CURSOR_BY_TIMER;

extern uint32_t CURSOR_UNKNOWN;
extern uint8_t CURSOR_IS_BLITTING;

extern int32_t MOUSE_BUTTONS_STATE;
extern "C" volatile int32_t MOUSE_BUTTONS_STATE1;

extern "C" volatile int32_t CURSOR_POS_X;
extern "C" volatile int32_t CURSOR_POS_Y;

extern "C" volatile int32_t CURSOR_POS_LCLICK_X;
extern "C" volatile int32_t CURSOR_POS_LCLICK_Y;
extern "C" volatile int32_t CURSOR_POS_RCLICK_X;
extern "C" volatile int32_t CURSOR_POS_RCLICK_Y;

extern "C" volatile int32_t MOUSE_CLICK;

enum CURSOR_ICON
{
    ICON_CROSSHAIR = 0,
    ICON_CURSOR = 93,
    ICON_WAIT = 144,
};

void mouse_update();
extern "C" int hide_cursor();
void get_cursor_pos(int *x, int *y);
extern "C" void show_cursor();
void bedlam_mouse_buttons(uint16_t state_1, uint16_t state_2);
extern "C" void set_cursor_icon(int icon);
bool mouse_right_button_pressed();
bool mouse_left_button_pressed();
void set_mouse_click();
void reset_mouse_click();
bool mouse_clicked();