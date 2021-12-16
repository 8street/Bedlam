#pragma once
#include <string>
#include <vector>

#include "resolutions.h"

#pragma pack(push, 1)
struct OPTIONS_BDL
{
    int32_t backbuffer = 1;
    int32_t actionpan = 1;
    int32_t language = 0;
    int32_t cd_audio = 1;
    char playername[8] = "Player";
    int32_t volume = 100;
    int32_t code_no_title = 1;
    int32_t midi = 1;
    int32_t sound = 1;
    char installdrive = 'C';
};
#pragma pack(pop)

class Options
{
public:
    Options();
    ~Options();
    const Resolution_settings &get_resolution_settings(const Resolution &res) const;
    int load_options_bdl_file();
    int32_t sound_is_enable() const;
    int32_t actionpan_is_enable() const;
    int32_t get_language() const;
    char get_installdrive() const;
    void get_playername(char *player_name) const;
    int32_t midi_is_enable() const;
    int32_t cd_audio_is_enable() const;
    int32_t get_volume() const;
    int save_options(int32_t volume, char *player_name);

private:
    const Resolution_settings &get_nearest_resolution_setting(const Resolution &res) const;
    int create_default_options_bdl();
    int save_options_to_file();
    int32_t get_system_language() const;
    std::vector<Resolution_settings> m_resolutions_settings;
    OPTIONS_BDL m_options_data;
    std::string m_options_bdl_path = "SAVES/OPTIONS.BDL";
};

extern Options GAME_OPTIONS;

extern "C" int32_t sound_is_enable();
extern "C" int32_t speechs_is_enable();
extern "C" int32_t cinematics_is_enable();
extern "C" int32_t actionpan_is_enable();
extern "C" int32_t get_language();
extern "C" int32_t get_misc();
extern "C" int32_t get_installdrive();
extern "C" void get_playername(char *player_name);
extern "C" int32_t midi_is_enable();
extern "C" int32_t cd_audio_is_enable();
extern "C" int32_t get_saved_master_volume();

extern "C" void save_options(int32_t volume, char* player_name);
