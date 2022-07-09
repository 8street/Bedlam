#include <iostream>

#include "sdl_event.h"
#include "sdl_sound.h"
#include "sdl_window.h"

#include "mouse.h"
#include "smk.h"
#include "timer.h"

Smack::Smack()
{
}

Smack::Smack(const std::string &filename)
{
    load(filename);
}

Smack::~Smack()
{
    destroy();
}

int Smack::load(const std::string &filename)
{
    if (m_smack_ptr)
    {
        destroy();
    }
    m_smack_ptr = smk_open_file(filename.c_str(), SMK_MODE_MEMORY);
    if (!m_smack_ptr)
    {
        std::cout << "Errors encountered opening " << filename << std::endl;
        return -1;
    }
    int ret_val = 0;
    ret_val |= smk_info_all(m_smack_ptr, NULL, &m_num_frames, &m_us_per_frame);
    ret_val |= smk_info_video(m_smack_ptr, &m_width, &m_height, NULL);
    ret_val |= smk_info_audio(m_smack_ptr, &m_track_mask, m_channels, m_bitrate, m_samplerate);
    if (ret_val)
    {
        destroy();
    }
    return ret_val;
}

int Smack::play()
{
    if (!m_smack_ptr)
    {
        return -1;
    }
    int ret_val = 0;
    ret_val |= enable_video_audio();

    // WINDOW_CURSOR.set_waiting_cursor();
    const int old_num_channels = SOUND_SYSTEM.get_last_channel_index();
    ret_val |= play_audio(get_first_existing_track());
    WINDOW_CURSOR.hide_cursor();
    // Audio delay
    SDL_Delay(20);
    ret_val |= play_video();
    SOUND_SYSTEM.free_unused_chunks(old_num_channels);
    return ret_val;
}

int Smack::destroy()
{
    if (m_smack_ptr)
    {
        smk_close(m_smack_ptr);
        m_smack_ptr = nullptr;
    }
    return 0;
}

int Smack::enable_video_audio()
{
    int ret_val = 0;
    ret_val |= smk_enable_video(m_smack_ptr, 1);
    int track;
    for (track = 0; track < MAX_TRACK; track++)
    {
        if (track_exist(track))
        {
            ret_val |= smk_enable_audio(m_smack_ptr, track, 1);
        }
    }
    return ret_val;
}

int Smack::get_video_pos_x() const
{
    int pos_x = (ORIGINAL_GAME_WIDTH - m_width) / 2;
    if (pos_x < 0)
    {
        pos_x = 0;
    }
    return pos_x;
}

int Smack::get_video_pos_y() const
{
    int pos_y = (ORIGINAL_GAME_HEIGHT - m_height) / 2;
    if (pos_y < 0)
    {
        pos_y = 0;
    }
    return pos_y;
}

int Smack::set_smack_palette()
{
    const unsigned char *palette_data = smk_get_palette(m_smack_ptr);
    return GAME_WINDOW.set_palette(const_cast<uint8_t *>(palette_data), 0, 256, Palette_mode::normal);
}

int Smack::play_video()
{
    if (!m_smack_ptr)
    {
        return -1;
    }
    int ret_val = 0;
    double time_error_ms = 0.0;
    smk_first(m_smack_ptr);
    unsigned long cur_frame;
    ret_val |= smk_info_all(m_smack_ptr, &cur_frame, NULL, NULL);
    const unsigned char *image_data = smk_get_video(m_smack_ptr);
    ret_val |= GAME_WINDOW.clear_screen();
    ret_val |= set_smack_palette();
    const int start_x = get_video_pos_x();
    const int start_y = get_video_pos_y();
    Timer frame_timer;
    // Output video
    for (cur_frame = 0; cur_frame < m_num_frames; cur_frame++)
    {
        frame_timer.reset();
        ret_val |= GAME_WINDOW.fill_screen_surface(
            const_cast<uint8_t *>(image_data), start_x, start_y, 0, 0, m_width, m_height, m_width);
        ret_val |= GAME_WINDOW.redraw();
        ret_val |= SDL_events();
        ret_val |= smk_next(m_smack_ptr);
        double delay_ms = m_us_per_frame / 1000.0 - frame_timer.elapsed() * 1000.0;
        if (delay_ms < 0.0)
        {
            delay_ms = 0.0;
        }
        time_error_ms += delay_ms - lround(delay_ms);
        if (time_error_ms > 1.0)
        {
            delay_ms += 1.0;
            time_error_ms -= 1.0;
        }
        else if (time_error_ms < -1.0)
        {
            delay_ms -= 1.0;
            time_error_ms += 1.0;
        }
        SDL_Delay(lround(delay_ms));
    }
    return ret_val;
}

int Smack::play_audio(int track)
{
    if (!m_smack_ptr)
    {
        return -1;
    }
    int ret_val = 0;
    smk_first(m_smack_ptr);
    unsigned long cur_frame;
    ret_val |= smk_info_all(m_smack_ptr, &cur_frame, NULL, NULL);
    std::vector<uint8_t> audio_data;
    fill_audio_data(audio_data, track);
    // Output audio
    const int audio_chunk_index = SOUND_SYSTEM.add_raw(
        audio_data.data(), audio_data.size(), m_samplerate[track], m_bitrate[track], m_channels[track]);
    ret_val |= SOUND_SYSTEM.play_sound(audio_chunk_index);
    //SOUND_SYSTEM.add_music(audio_data.data(), audio_data.size(), m_samplerate[track], m_bitrate[track], m_channels[track]);
    //SOUND_SYSTEM.play_music();
    return ret_val;
}

int Smack::fill_audio_data(std::vector<uint8_t> &audio_data, int track) const
{
    if (!track_exist(track))
    {
        return -1;
    }
    int ret_val = 0;
    audio_data.clear();
    smk_first(m_smack_ptr);
    unsigned long cur_frame;
    ret_val |= smk_info_all(m_smack_ptr, &cur_frame, NULL, NULL);
    unsigned long frame_audio_size = 0;
    const unsigned char *frame_audiodata = nullptr;
    // get audio data
    for (cur_frame = 0; cur_frame < m_num_frames; cur_frame++)
    {
        frame_audio_size = smk_get_audio_size(m_smack_ptr, track);
        frame_audiodata = smk_get_audio(m_smack_ptr, track);
        audio_data.insert(audio_data.end(), frame_audiodata, frame_audiodata + frame_audio_size);
        smk_next(m_smack_ptr);
        SDL_events();
    }
    return ret_val;
}

bool Smack::track_exist(int track) const
{
    return m_track_mask & (1 << track);
}

int Smack::get_first_existing_track() const
{
    for (int track = 0; track < MAX_TRACK; track++)
    {
        if (track_exist(track))
        {
            return track;
        }
    }
    return 0;
}