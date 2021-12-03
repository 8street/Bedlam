#pragma once
#include <cstdint>

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

class Save
{
public:
    Save();
    ~Save();



    int load_options();
    int save_options();

    int load_hiscores();
    int save_hiscores();

private:
    OPTIONS_BDL m_options_data;
    uint8_t m_savegames_data[900] = { 0 };
    uint8_t m_hiscores_data[120] = { 0 };
};
