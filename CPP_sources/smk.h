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
    int play();

private:
    int destroy();
    int enable_video_audio();
    int get_video_pos_x() const;
    int get_video_pos_y() const;
    int set_smack_palette();
    int play_video();
    int play_audio(int track);
    int fill_audio_data(std::vector<uint8_t> &audio_data, int track) const;
    bool track_exist(int track) const;
    int get_first_existing_track() const;
    int wait_next_frame(Timer &frame_timer) const;

    smk m_smack_ptr = nullptr;
    unsigned long m_width = 0, m_height = 0, m_num_frames = 0;
    double m_us_per_frame = 0;
    unsigned char m_track_mask = 0, m_channels[MAX_TRACK] = {}, m_bitrate[MAX_TRACK] = {};
    unsigned long m_samplerate[MAX_TRACK] = {};
};