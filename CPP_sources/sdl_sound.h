#pragma once
#include <SDL_mixer.h>

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "music_chunk.h"
#include "sound_chunk.h"

class Sound
{
public:
    Sound();
    ~Sound();
    int init();
    int add_raw(uint8_t *raw_ptr, int filesize, int samplerate = 11025, int bitrate = 8, int num_channels = 1);
    int add_music(uint8_t *raw_ptr, int filesize, int samplerate = 11025, int bitrate = 8, int num_channels = 1);
    int play_sound(int channel_index, int x = -1, int y = -1, bool loop = false);
    int play_raw(int channel_index, int position, int samplerate, int volume, int balance);
    int play_music();
    int stop();
    int stop(int channel_index);
    int fade_stop(int ms);
    int fade_stop(int channel_index, int ms);
    int set_volume(int new_volume);
    int is_stopped(int channel_index) const;
    int free_unused_chunks(int new_channels_count);
    int get_last_channel_index();

private:
    int get_chunk_index(const std::string &path) const;
    int get_first_free_channel(int chunk_index) const;
    int get_last_chunk_index() const;
    int get_volume(int x, int y) const;
    int get_balance(int x, int y) const;
    int get_channel_index(int chunk_index) const;
    int get_chunk_index(int channel_index) const;
    std::vector<Sound_chunk> m_chunks_arr;
    Music_chunk m_music;
    std::unordered_map<std::string, int> m_filename_index_map;
    int m_num_simultaneously_playing_channels = 0;
    int m_master_volume = 0;
    bool m_sound_was_initted = false;
};

extern Sound SOUND_SYSTEM;

//////// Function calls from bedlam2.asm ///////////

extern "C" int play_sound(int channel_index, int x, int y, int a5);
extern "C" int play_music(int channel_index, int x, int y, int flag);
extern "C" void stop_music();
extern "C" void stop_sound(int channel_index);
extern "C" void set_volume(int volume);
extern "C" int sound_is_stopped(int channel_index);
extern "C" int load_raw_to_soundbufer(uint8_t *raw_ptr, int filesize, int samplerate, int bitrate, int num_channels);
extern "C" int free_unused_sound_buffers(int new_buffer_count);
extern "C" int play_sound2(int channel_index, int position, int samplerate, int volume, int balance);