#include <cstdint>
#include <iostream>

#include "sdl_event.h"
#include "sdl_sound.h"
#include "sdl_window.h"
#include "smacker.h"
#include "smk.h"
#include "timer.h"

int play_smack(const char *filename)
{
    unsigned long smk_width, smk_height, smk_num_frames;
    double smk_us_per_frame;
    smk smack = nullptr;

    smack = smk_open_file(filename, SMK_MODE_MEMORY);
    if (!smack)
    {
        std::cout << "Errors encountered opening " << filename << ", exiting." << std::endl;
        return -1;
    }

    /* print some info about the file */
    smk_info_all(smack, NULL, &smk_num_frames, &smk_us_per_frame);
    smk_info_video(smack, &smk_width, &smk_height, NULL);
    // std::cout << "Opened file" << filename << " Width:" << smk_width << " Height:" << smk_height << " Frames:" <<
    // smk_num_frames
    //          << " FPS:" << 1000000.0 / smk_us_per_frame << std::endl;

    unsigned char track_mask, smk_channels[7], smk_bitrate[7];
    unsigned long smk_samplerate[7];
    smk_info_audio(smack, &track_mask, smk_channels, smk_bitrate, smk_samplerate);
    int track;
    // for (track = 0; track < 7; track++)
    //{
    //    std::cout << "Audio track " << track << ": " << smk_bitrate[track] << " bits, " << smk_channels[track] << " channels, " <<
    //    smk_samplerate[track] << " hz."
    //              << std::endl;
    //}

    /* Turn on decoding for palette, video, and audio tracks */
    smk_enable_video(smack, 1);
    for (track = 0; track < 7; track++)
    {
        if (track_mask & (1 << track))
        {
            smk_enable_audio(smack, track, 1);
        }
    }

    // Get a pointer to first frame
    smk_first(smack);
    unsigned long cur_frame;
    smk_info_all(smack, &cur_frame, NULL, NULL);

    const unsigned char *audio_data = smk_get_audio(smack, 0);
    unsigned long audio_size = smk_get_audio_size(smack, 0);
    const int old_num_channels = SOUND_SYSTEM.get_last_channel_index();

    const unsigned char *palette_data = smk_get_palette(smack);
    const unsigned char *image_data = smk_get_video(smack);
    GAME_WINDOW.set_palette(const_cast<uint8_t *>(palette_data), 0, 256, Palette_mode::normal);
    int start_x = (ORIGINAL_GAME_WIDTH - smk_width) / 2;
    if (start_x < 0)
    {
        start_x = 0;
    }
    int start_y = (ORIGINAL_GAME_HEIGHT - smk_height) / 2;
    if (start_y < 0)
    {
        start_y = 0;
    }
    GAME_WINDOW.clear_screen();
    Timer frame_timer;
    for (cur_frame = 0; cur_frame < smk_num_frames; cur_frame++)
    {
        frame_timer.reset();
        // Output audio
        SOUND_SYSTEM.free_unused_chunks(old_num_channels + 30);
        for (track = 0; track < 7; track++)
        {
            if (track_mask & (1 << track))
            {
                audio_size = smk_get_audio_size(smack, track);
                const int chunk = SOUND_SYSTEM.add_raw(
                    const_cast<uint8_t *>(audio_data), audio_size, smk_samplerate[track], smk_bitrate[track], smk_channels[track]);
                SOUND_SYSTEM.play_sound(chunk);
            }
        }
        // Output video
        GAME_WINDOW.fill_screen_surface(
            const_cast<uint8_t *>(image_data), start_x, start_y, 0, 0, smk_width, smk_height, smk_width);
        GAME_WINDOW.redraw();
        SDL_events();
        smk_next(smack);
        double delay_ms = smk_us_per_frame / 1000.0 - frame_timer.elapsed() * 1000.0;
        if (delay_ms < 0.0)
        {
            delay_ms = 0.0;
        }
        SDL_Delay(lround(delay_ms));
    }
    SOUND_SYSTEM.free_unused_chunks(old_num_channels);
    smk_close(smack);
    return 0;
}