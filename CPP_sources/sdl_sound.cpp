#include <SDL.h>
#include <iostream>

#include "bedlam.h"
#include "bedlam_draw.h"
#include "helper.h"
#include "sdl_event.h"
#include "sdl_sound.h"

#ifdef _MSC_VER
#    pragma warning(disable : 4005)
#endif

#define MIX_MAX_BALANCE 255
#define MIX_CHANNELS 10;

Sound SOUND_SYSTEM;

Sound::Sound()
{
}

Sound::~Sound()
{
    Mix_CloseAudio();
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int Sound::init()
{
    int ret_val = 0;

    if (SDL_Init(SDL_INIT_AUDIO))
    {
        std::cout << "ERROR: SDL init audio. " << SDL_GetError() << std::endl;
        ret_val |= -1;
    }

    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 128))
    {
        std::cout << "ERROR: Mix_OpenAudio. " << Mix_GetError() << std::endl;
        ret_val |= -1;
    }
    m_num_simultaneously_playing_channels = MIX_CHANNELS;
    // Allocate check
    int max_channels = 156 * m_num_simultaneously_playing_channels;
    int num_channels = Mix_AllocateChannels(max_channels);
    int num_reserve_channels = Mix_ReserveChannels(max_channels);
    if (num_channels != max_channels || num_reserve_channels != max_channels)
    {
        std::cout << "ERROR: allocate channels. Current channels number is " << num_channels << std::endl;
        ret_val |= -1;
    }

    m_master_volume = 100;
    if (Mix_Volume(-1, MIX_MAX_VOLUME) != MIX_MAX_VOLUME)
    {
        ret_val |= -1;
    }

    if (!ret_val)
    {
        m_sound_was_initted = true;
    }

    return ret_val;
}

int Sound::add_raw(const std::string &path)
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    // if we already have this file, just return file index
    if (get_chunk_index(path) >= 0)
    {
        return get_chunk_index(path);
    }
    m_chunks_arr.emplace_back(path);
    SDL_events();
    const int chunk_index = get_last_chunk_index();
    m_filename_index_map.emplace(path, chunk_index);
    Mix_VolumeChunk(m_chunks_arr[chunk_index].get_chunk(), MIX_MAX_VOLUME);
    Mix_AllocateChannels(m_num_simultaneously_playing_channels * (chunk_index + 1));
    return chunk_index;
}

int Sound::play_raw(int index, int x, int y, bool loop)
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    int ret_val = 0;
    int balance = 0;
    int l_balance = 0;
    int r_balance = 0;
    int volume = 0;
    int palying_times = 0;

    if (x == -1 && y == -1)
    {
        volume = MIX_MAX_VOLUME;
        l_balance = MIX_MAX_BALANCE;
        r_balance = MIX_MAX_BALANCE;
    }
    else
    {
        volume = get_volume(x, y) >> 8;   // max 128
        balance = get_balance(x, y) >> 7; // max 255
        if (balance > 0)
        {
            l_balance = balance - MIX_MAX_BALANCE;
            r_balance = MIX_MAX_BALANCE;
        }
        else
        {
            l_balance = MIX_MAX_BALANCE;
            r_balance = balance + MIX_MAX_BALANCE;
        }
    }
    volume = volume * m_master_volume / 100;

    int free_channel_index = get_first_free_channel(index);

    Mix_Volume(free_channel_index, volume);
    ret_val |= !Mix_SetPanning(free_channel_index, static_cast<uint8_t>(l_balance), static_cast<uint8_t>(r_balance));
    if (loop)
    {
        palying_times = -1;
    }
    Mix_PlayChannel(free_channel_index, m_chunks_arr[index].get_chunk(), palying_times);
    return ret_val;
}

int Sound::stop()
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    return Mix_HaltChannel(-1);
}
int Sound::stop(int index)
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    return Mix_HaltChannel(index);
}

int Sound::fade_stop(int ms)
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    return Mix_FadeOutChannel(-1, ms);
}

int Sound::fade_stop(int index, int ms)
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    return Mix_FadeOutChannel(index, ms);
}

int Sound::set_volume(int new_volume)
{
    if (new_volume > 100)
    {
        new_volume = 100;
    }
    else if (new_volume < 0)
    {
        new_volume = 0;
    }
    m_master_volume = new_volume;
    return 0;
}

int Sound::is_stopped(int channel_index) const
{
    return !Mix_Playing(channel_index);
}

int Sound::get_chunk_index(const std::string &path) const
{
    auto search = m_filename_index_map.find(path);
    if (search != m_filename_index_map.end())
    {
        return search->second;
    }
    return -1;
}

int Sound::get_last_chunk_index() const
{
    return static_cast<int>(m_chunks_arr.size()) - 1;
}

int Sound::get_first_free_channel(int index) const
{
    for (int i = index; i < index + m_num_simultaneously_playing_channels; i++)
    {
        if (is_stopped(i))
        {
            return i;
        }
    }
    Mix_HaltChannel(index);
    return index;
}

int Sound::get_volume(int x, int y) const
{
    // Displace screen pos for all screen resolutions
    const int real_scr_x = screen_x_pos - DISPLACE_SCREEN_X;
    const int real_scr_y = screen_y_pos - DISPLACE_SCREEN_Y;
    const int decrease_factor = 20480 / SCREEN_SURFACE_WIDTH;
    int volume = 32768
        - decrease_factor * get_distance(y - real_scr_y - (x - real_scr_x), -((x - real_scr_x + y - real_scr_y) >> 1));
    if (volume < 0)
    {
        volume = 0;
    }
    else if (volume > 32768)
    {
        volume = 32768;
    }
    return volume;
}
int Sound::get_balance(int x, int y) const
{
    // Displace screen pos for all screen resolutions
    const int real_scr_x = screen_x_pos - DISPLACE_SCREEN_X;
    const int real_scr_y = screen_y_pos - DISPLACE_SCREEN_Y;
    const int decrease_factor = 143360 / SCREEN_SURFACE_WIDTH;
    int balance = -decrease_factor * (y - real_scr_y - (x - real_scr_x));
    if (balance < -32640)
    {
        balance = -32640;
    }
    else if (balance > 32640)
    {
        balance = 32640;
    }
    return balance;
}

//////// Function calls from bedlam2.asm ///////////

int load_raw(const char *filename)
{
    return SOUND_SYSTEM.add_raw(filename);
}

int load_music(const char *filename)
{
    return SOUND_SYSTEM.add_raw(filename);
}

int play_sound(int raw_index, int x, int y, int a5)
{
    return SOUND_SYSTEM.play_raw(raw_index, x, y);
}

int play_music(int raw_index, int x, int y, int flag)
{
    return SOUND_SYSTEM.play_raw(raw_index, x, y, true);
}

void stop_music()
{
    SOUND_SYSTEM.fade_stop(10);
}

void stop_sound(int channel_index)
{
    SOUND_SYSTEM.stop(channel_index);
}

void set_volume(int volume)
{
    SOUND_SYSTEM.set_volume(volume);
}

int sound_is_stopped(int channel_index)
{
    return SOUND_SYSTEM.is_stopped(channel_index);
}