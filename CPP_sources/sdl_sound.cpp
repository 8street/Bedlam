#include <SDL.h>
#include <iostream>

#include "bedlam.h"
#include "bedlam_draw.h"
#include "helper.h"
#include "options.h"
#include "sdl_event.h"
#include "sdl_sound.h"

#define MIX_MAX_BALANCE 255

#undef MIX_CHANNELS
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
    m_num_simultaneously_playing_channels = 6;
    // Allocate check
    const int num_sound_files = 160;
    m_chunks_arr.reserve(num_sound_files);
    const int max_channels = get_channel_index(num_sound_files);
    const int num_channels = Mix_AllocateChannels(max_channels);
    const int num_reserve_channels = Mix_ReserveChannels(max_channels);
    if (num_channels != max_channels || num_reserve_channels != max_channels)
    {
        std::cout << "ERROR: allocate channels. Current channels number is " << num_channels << std::endl;
        ret_val |= -1;
    }

    m_master_volume = GAME_OPTIONS.get_volume();
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

int Sound::add_raw(uint8_t *raw_ptr, int filesize, int samplerate, int bitrate, int num_channels)
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    m_chunks_arr.push_back(Sound_chunk(WAV(raw_ptr, filesize, samplerate, bitrate, num_channels)));
    SDL_events();
    const int chunk_index = get_last_chunk_index();
    Mix_VolumeChunk(m_chunks_arr[chunk_index].get_chunk(), MIX_MAX_VOLUME);
    return get_channel_index(chunk_index);
}

int Sound::play_sound(int channel_index, int x, int y, bool loop)
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    int balance = 0;
    int l_balance = 0;
    int r_balance = 0;
    int volume = 0;
    int palying_times = 0;
    int chunk_index = get_chunk_index(channel_index);
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

    int free_channel_index = get_first_free_channel(chunk_index);

    Mix_Volume(free_channel_index, volume);
    Mix_SetPanning(free_channel_index, static_cast<uint8_t>(l_balance), static_cast<uint8_t>(r_balance));
    if (loop)
    {
        palying_times = -1;
    }
    Mix_PlayChannel(free_channel_index, m_chunks_arr[chunk_index].get_chunk(), palying_times);
    return free_channel_index;
}

int Sound::play_raw(int channel_index, int position, int samplerate, int volume, int balance)
{
    if (!m_sound_was_initted)
    {
        return -1;
    }
    int l_balance = 0;
    int r_balance = 0;
    int palying_times = 0;
    int chunk_index = get_chunk_index(channel_index);

    volume = volume * m_master_volume / 100;
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
    m_chunks_arr[chunk_index].resample(samplerate);
    int free_channel_index = get_first_free_channel(chunk_index);
    Mix_Volume(free_channel_index, volume);
    Mix_SetPanning(free_channel_index, static_cast<uint8_t>(l_balance), static_cast<uint8_t>(r_balance));
    Mix_PlayChannel(free_channel_index, m_chunks_arr[chunk_index].get_chunk(), palying_times);
    return free_channel_index;
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
    if (!m_sound_was_initted)
    {
        return 1;
    }
    return !Mix_Playing(channel_index);
}

int Sound::free_unused_chunks(int new_channels_count)
{
    const int new_chunks_count = get_chunk_index(new_channels_count);
    if (new_chunks_count > get_last_chunk_index())
    {
        return get_channel_index(get_last_chunk_index());
    }
    stop();
    m_chunks_arr.resize(new_chunks_count + 1);
    return new_channels_count;
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

int Sound::get_first_free_channel(int chunk_index) const
{
    const int channel_index = get_channel_index(chunk_index);
    for (int current_channel = channel_index; current_channel < channel_index + m_num_simultaneously_playing_channels; current_channel++)
    {
        if (is_stopped(current_channel))
        {
            return current_channel;
        }
    }
    Mix_HaltChannel(channel_index);
    return channel_index;
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

int Sound::get_channel_index(int chunk_index) const
{
    return chunk_index * m_num_simultaneously_playing_channels;
}

int Sound::get_chunk_index(int channel_index) const
{
    return channel_index / m_num_simultaneously_playing_channels;
}

//////// Function calls from bedlam2.asm ///////////

int play_sound(int channel_index, int x, int y, int a5)
{
    return SOUND_SYSTEM.play_sound(channel_index, x, y);
}

int play_music(int channel_index, int x, int y, int flag)
{
    return SOUND_SYSTEM.play_sound(channel_index, x, y, true);
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

int load_raw_to_soundbufer(uint8_t *raw_ptr, int filesize, int samplerate, int bitrate, int num_channels)
{
    return SOUND_SYSTEM.add_raw(raw_ptr, filesize, samplerate, bitrate, num_channels);
}

int free_unused_sound_buffers(int new_buffer_count)
{
    return SOUND_SYSTEM.free_unused_chunks(new_buffer_count);
}

int play_sound2(int channel_index, int position, int samplerate, int volume, int balance)
{
    samplerate = samplerate * 44100ll / 0x10000ll;
    volume = volume * MIX_MAX_VOLUME / 0x8000;
    balance = balance * MIX_MAX_BALANCE / 0x8000;
    return SOUND_SYSTEM.play_raw(channel_index, position, samplerate, volume, balance);
}