#include <SDL.h>
#include <iostream>

#include "bedlam.h"
#include "main.h"
#include "mouse.h"
#include "sdl_sound.h"
#include "sdl_timer.h"
#include "sdl_window.h"

int main(int argc, char *argv[])
{
#ifdef _DEBUG
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    atexit([] { system("PAUSE"); });
#endif

    if (GAME_WINDOW.init())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Problem", "Error init video!", NULL);
        return 1;
    }
    if (WINDOW_CURSOR.init())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Problem", "Error init cursor!", NULL);
    }
    // timer changed to 9 for more smoothly performance
    if (GAME_TIMER.init(9))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Problem", "Error init timer!", NULL);
        return 2;
    }
    if (SOUND_SYSTEM.init())
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Problem", "Error init sound!", NULL);
    }

    // Run bedlam main function
    return game_core1();
}

void free_win()
{
    // Place all destroy and deinits here

    // Bedlam free from ASM
    free_mem();
}
