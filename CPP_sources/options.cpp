#include <algorithm>
#include <iostream>

#include <SDL.h>

#include "file.h"
#include "options.h"

Options GAME_OPTIONS;

Options::Options()
{
    // Add your resolution setting here in following format
    // m_resolutions_settings.push_back(Resolution_settings(Resolution(x,  y),  - screen resolution
    // num_tiles_x, num_tiles_y,                                                - number tiles
    // not_visible_tiles_x,  not_visible_tiles_x,                               - not visible tiles
    //                                                                            or visible tiles from robot pos to screen end
    //                                                                            I not sure
    // tiles_start_pos_x, tiles_start_pos_y,                                    - all tiles represent a rhombus with a beginning
    //                                                                            position at the top of rhombus
    // displace_origin_y));                                                     - displace vertical robot origin for correct
    // robot
    //                                                                            top part angle depending on mouse position
    //
    // You not need to change default tiles_start_pos_x value 304
    //
    // For correct working vertical offset
    // tiles_start_pos_y should be aliquot to value TILE_HEIGHT = 32

    m_resolutions_settings.push_back(Resolution_settings(Resolution(640, 480), 36, 36, 9, 9, 304, -256, 16));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(800, 600), 46, 36, 12, 12, 304, -256, 32));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(1024, 768), 56, 46, 15, 15, 304, -256, 62));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(1366, 768), 66, 56, 16, 18, 304, -448, -16));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(1440, 900), 70, 60, 16, 18, 304, -448, 28));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(1280, 1024), 70, 60, 16, 18, 304, -384, 132));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(1600, 900), 70, 60, 16, 18, 304, -448, -16));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(1600, 1200), 80, 65, 16, 26, 304, -512, 132));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(1920, 1080), 90, 70, 16, 32, 304, -736, -6));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(1920, 1200), 100, 70, 16, 32, 304, -736, 56));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(2048, 1080), 100, 70, 16, 36, 304, -768, -32));
    m_resolutions_settings.push_back(Resolution_settings(Resolution(2560, 1440), 120, 90, 16, 36, 304, -1056, 16));

    std::sort(m_resolutions_settings.begin(), m_resolutions_settings.end());

    load_options_bdl_file();
}

Options::~Options()
{
}

const Resolution_settings &Options::get_resolution_settings(const Resolution &res) const
{
    return get_nearest_resolution_setting(res);
}

const Resolution_settings &Options::get_nearest_resolution_setting(const Resolution &res) const
{
    Resolution_settings tmp_setting(res);
    auto const it = std::lower_bound(m_resolutions_settings.begin(), m_resolutions_settings.end(), tmp_setting);
    if (it == m_resolutions_settings.end())
    {
        return *(it - 1);
    }
    return *it;
}

int Options::load_options_bdl_file()
{
    create_default_options_bdl();
    File options_bdl(m_options_bdl_path);
    if (options_bdl.get_size() != sizeof(m_options_data))
    {
        std::cout << "ERROR: unable to load options. " << m_options_bdl_path << " file is corrypted. "
                  << "Expected file size: " << sizeof(m_options_data) << ". Existed file size: " << options_bdl.get_size()
                  << std::endl;
        return -1;
    }
    std::copy(
        options_bdl.get_ptr(), options_bdl.get_ptr() + options_bdl.get_size(), reinterpret_cast<uint8_t *>(&m_options_data));
    return 0;
}

int32_t Options::sound_is_enable() const
{
    return m_options_data.sound;
}

int32_t Options::actionpan_is_enable() const
{
    return m_options_data.actionpan;
}

int32_t Options::get_language() const
{
    return m_options_data.language;
}

char Options::get_installdrive() const
{
    return m_options_data.installdrive;
}

void Options::get_playername(char *player_name) const
{
    std::copy(m_options_data.playername, m_options_data.playername + sizeof(m_options_data.playername), player_name);
}

int32_t Options::midi_is_enable() const
{
    return m_options_data.midi;
}

int32_t Options::cd_audio_is_enable() const
{
    return m_options_data.cd_audio;
}

int32_t Options::get_volume() const
{
    return m_options_data.volume;
}

int Options::create_default_options_bdl()
{
    if (File::exist(m_options_bdl_path))
    {
        return -1;
    }
    m_options_data.language = get_system_language();
    save_options_to_file();
    return 0;
}

int Options::save_options_to_file()
{
    const uint8_t *data_begin = reinterpret_cast<uint8_t *>(&m_options_data);
    File options_bdl(m_options_bdl_path, data_begin, sizeof(m_options_data));
    return 0;
}

int Options::save_options(int32_t volume, char *player_name)
{
    m_options_data.volume = volume;
    std::copy(player_name, player_name + sizeof(m_options_data.playername), m_options_data.playername);
    return save_options_to_file();
}

int32_t Options::get_system_language() const
{
    // 0 - us uk
    // 1 - ger
    // 2 - spain
    // 3 - fre
    // 4 - itl
    // 5 - dutch
    int32_t language = 0;
    SDL_Locale *locales = SDL_GetPreferredLocales();
    const SDL_Locale *locales_start_address = locales;
    if (!locales)
    {
        std::cout << "ERROR: Couldn't read preffered locales. " << SDL_GetError() << std::endl;
        return language;
    }
    while (locales->language)
    {
        if (!_stricmp(locales->language, "en"))
        {
            language = 0;
            break;
        }
        else if (!_stricmp(locales->language, "de"))
        {
            language = 1;
            break;
        }
        else if (!_stricmp(locales->language, "es"))
        {
            language = 2;
            break;
        }
        else if (!_stricmp(locales->language, "fr"))
        {
            language = 3;
            break;
        }
        else if (!_stricmp(locales->language, "it"))
        {
            language = 4;
            break;
        }
        else if (!_stricmp(locales->language, "nl"))
        {
            language = 5;
            break;
        }
        locales++;
    }
    SDL_free(const_cast<SDL_Locale *>(locales_start_address));
    return language;
}

////////////////////////////////////////////////////////////

int32_t sound_is_enable()
{
    return GAME_OPTIONS.sound_is_enable();
}

int32_t speechs_is_enable()
{
    // So file OPTION.BDL does not contain speechs then is always on
    return 1;
}

int32_t cinematics_is_enable()
{
    // So file OPTION.BDL does not contain cinematics then is always on
    return 1;
}

int32_t actionpan_is_enable()
{
    return GAME_OPTIONS.actionpan_is_enable();
}

int32_t get_language()
{
    return GAME_OPTIONS.get_language();
}

int32_t get_misc()
{
    // seems is censorship selector
    // or UK/US selector
    return 0;
}

int32_t get_installdrive()
{
    return GAME_OPTIONS.get_installdrive();
}

void get_playername(char *player_name)
{
    GAME_OPTIONS.get_playername(player_name);
}

int32_t midi_is_enable()
{
    return GAME_OPTIONS.midi_is_enable();
}

int32_t cd_audio_is_enable()
{
    return GAME_OPTIONS.cd_audio_is_enable();
}

int32_t get_saved_master_volume()
{
    return GAME_OPTIONS.get_volume();
}

void save_options(int32_t volume, char *player_name)
{
    GAME_OPTIONS.save_options(volume, player_name);
}