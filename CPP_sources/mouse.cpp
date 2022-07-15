#include <SDL.h>

#include "bedlam.h"
#include "bedlam_draw.h"
#include "helper.h"
#include "mouse.h"
#include "sdl_window.h"

cursor WINDOW_CURSOR;

int16_t CURSOR_X1;
int16_t CURSOR_Y1;

volatile uint8_t UPDATE_CURSOR_BY_TIMER;

volatile int32_t CURSOR_POS_X;
volatile int32_t CURSOR_POS_Y;

volatile int32_t CURSOR_POS_LCLICK_X;
volatile int32_t CURSOR_POS_LCLICK_Y;
volatile int32_t CURSOR_POS_RCLICK_X;
volatile int32_t CURSOR_POS_RCLICK_Y;

int32_t MOUSE_BUTTONS_STATE;
volatile int32_t MOUSE_BUTTONS_STATE1;

volatile int32_t MOUSE_CLICK;

int32_t MOUSE_UP;
bool MOUSE_IS_CLICKED;

// 00425AB9 Bedlam 1
void mouse_update()
{
    int pos_x; // [esp+0h] [ebp-1Ch] BYREF
    int pos_y; // [esp+4h] [ebp-18h] OVERLAPPED BYREF
    uint32_t prev_state;

    prev_state = MOUSE_CLICK;
    if (!MOUSE_CLICK)
    {
        if ((MOUSE_BUTTONS_STATE1 & 3) != 0)
        {
            if (MOUSE_UP)
            {
                MOUSE_CLICK = 1;
                MOUSE_UP = prev_state;
            }
        }
        else
        {
            MOUSE_UP = 1;
        }
    }
    MOUSE_BUTTONS_STATE1 = MOUSE_BUTTONS_STATE;
    get_cursor_pos(&pos_x, &pos_y);
    CURSOR_POS_X = pos_x;
    CURSOR_POS_Y = pos_y;
    if (mouse_left_button_pressed())
    {
        CURSOR_POS_LCLICK_X = CURSOR_POS_X;
        CURSOR_POS_LCLICK_Y = CURSOR_POS_Y;
    }
    if (mouse_right_button_pressed())
    {
        CURSOR_POS_RCLICK_X = CURSOR_POS_X;
        CURSOR_POS_RCLICK_Y = CURSOR_POS_Y;
    }

    if (game_is_playing)
    {
        if (CURSOR_POS_X < SIDEBAR_START_POS_X)
        {
            set_cursor_icon(ICON_CROSSHAIR);
        }
        else
        {
            set_cursor_icon(ICON_CURSOR);
        }
    }
}

int hide_cursor()
{
    return WINDOW_CURSOR.hide_cursor();
}

void get_cursor_pos(int *x, int *y)
{
    SDL_GetMouseState(x, y);
    if (game_is_playing)
    {
        *x = *x * GAME_WINDOW.get_game_width() / GAME_WINDOW.get_window_width();
        *y = *y * GAME_WINDOW.get_game_height() / GAME_WINDOW.get_window_height();
    }
    else
    {
        int menu_width = GAME_WINDOW.get_window_height() * 4 / 3;
        int menu_start_pos_x = (GAME_WINDOW.get_window_width() - menu_width) / 2;
        *x = (*x - menu_start_pos_x) * ORIGINAL_GAME_WIDTH / menu_width;
        *y = *y * ORIGINAL_GAME_HEIGHT / GAME_WINDOW.get_window_height();
    }
}

void show_cursor()
{
    WINDOW_CURSOR.show_cursor();
}

// 0041BF35 Bedlam 1
void bedlam_mouse_buttons(uint16_t state_1, uint16_t state_2)
{
    if (!state_1 && !state_2)
    {
        MOUSE_BUTTONS_STATE = set_bit(MOUSE_BUTTONS_STATE, 0);
    }
    if (!state_1 && state_2)
    {
        MOUSE_BUTTONS_STATE = reset_bit(MOUSE_BUTTONS_STATE, 0);
    }
    if (state_1 && !state_2)
    {
        MOUSE_BUTTONS_STATE = set_bit(MOUSE_BUTTONS_STATE, 1);
    }
    if (state_1 && state_2)
    {
        MOUSE_BUTTONS_STATE = reset_bit(MOUSE_BUTTONS_STATE, 1);
    }
}

void set_cursor_icon(int icon)
{
    WINDOW_CURSOR.set_cursor_icon(icon);
}

bool mouse_right_button_pressed()
{
    return check_bit(MOUSE_BUTTONS_STATE, 1);
}

bool mouse_left_button_pressed()
{
    return check_bit(MOUSE_BUTTONS_STATE, 0);
}

void set_mouse_click()
{
    MOUSE_IS_CLICKED = true;
}

void reset_mouse_click()
{
    MOUSE_IS_CLICKED = false;
}

bool mouse_clicked()
{
    bool ret_val = MOUSE_IS_CLICKED;
    MOUSE_IS_CLICKED = false;
    return ret_val;
}