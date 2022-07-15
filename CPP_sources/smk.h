#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "smacker.h"
#include "timer.h"

// .SMK have 7 tracks
#define MAX_TRACK 7

class Smack
{
public:
    Smack();
    Smack(const std::string &filename);
    ~Smack();

    int load(const std::string &filename);
    int play(bool is_skippable = true);
    int wait_next_frame();
    int next_frame();
    int encode_frame();
    int video_frame_to_buffer(uint8_t *buffer, int buffer_width, int buffer_height);
    int enable_video_audio();
    int play_audio();
    int set_smack_palette();

private:
    int destroy();
    int get_video_pos_x() const;
    int get_video_pos_y() const;
    int play_video(bool is_skippable);
    int play_audio(int track);
    int fill_audio_data(std::vector<uint8_t> &audio_data, int track) const;
    bool track_exist(int track) const;
    int get_first_existing_track() const;

    smk m_smack_ptr = nullptr;
    unsigned long m_width = 0, m_height = 0, m_num_frames = 0;
    double m_us_per_frame = 0;
    unsigned char m_track_mask = 0, m_channels[MAX_TRACK] = {}, m_bitrate[MAX_TRACK] = {};
    unsigned long m_samplerate[MAX_TRACK] = {};
    Timer m_frame_timer;
    int m_num_channels_old = 0;
};

//////// Function calls from bedlam.asm ///////////
extern "C" int play_smack(const char *filename, int32_t vertical_indent, int32_t is_skippable);
extern "C" Smack *open_smack(const char *filename);
extern "C" void smack_close(Smack *video);
extern "C" void smack_wait(Smack *video);
extern "C" void smack_to_buffer(
    Smack *video, uint32_t left, uint32_t top, uint32_t pitch, uint32_t destheight, uint8_t *out_buffer, uint32_t reversed);
extern "C" void smack_next_frame(Smack *video);