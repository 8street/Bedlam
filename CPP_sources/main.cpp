#include <SDL.h>
#include <iostream>

#include "bedlam.h"
#include "main.h"
#include "mouse.h"
#include "sdl_sound.h"
#include "sdl_timer.h"
#include "sdl_window.h"
#include "smk.h"


int main(int argc, char *argv[])
{
#ifdef _DEBUG
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    atexit([] { system("PAUSE"); });
#endif

    if (!File::exist("GAMEGFX/GAMEPAL.PAL") || !File::exist("EDITOR/ZONEA/MISSIONA.BIN"))
    {
        const char error_str[] = "Seems some game files are missing.\nYou need original Bedlam game (DOS or Win "
                                 "version).\nPlace executable file in your Bedlam game folder.";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Problem", error_str, NULL);
        return 404;
    }
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

    while (1)
    {
        //play_smack("GAMEGFX/TITLE.SMK");
        Smack title("GAMEGFX/TITLE.SMK");
        title.play();
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
