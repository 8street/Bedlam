#pragma once
#include <cstdint>

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

    uint8_t m_savegames_data[900] = { 0 };
    uint8_t m_hiscores_data[120] = { 0 };
};
