#include <iostream>

#include "file.h"
#include "save.h"

// Original file sizes. Dont change!
#define SAVES_FILE_SIZE 900
#define HISCORES_FILE_SIZE 120

#define SAVES_PATH "SAVES/SAVED.BDL"
#define HISCORE_PATH "SAVES/HISCORE.BDL"

int get_saves(uint8_t *save_data)
{
    return copy_data_from_file(SAVES_PATH, save_data, SAVES_FILE_SIZE);
}

int saves_to_disk(uint8_t *save_data)
{
    return save_data_to_file(SAVES_PATH, save_data, SAVES_FILE_SIZE);
}

int get_hiscores(uint8_t *hiscores_data)
{
    return copy_data_from_file(HISCORE_PATH, hiscores_data, HISCORES_FILE_SIZE);
}

int hiscores_to_disk(uint8_t *hiscores_data)
{
    return save_data_to_file(HISCORE_PATH, hiscores_data, HISCORES_FILE_SIZE);
}

int saves_exist()
{
    return File::exist(SAVES_PATH);
}

int hiscores_exist()
{
    return File::exist(HISCORE_PATH);
}

int copy_data_from_file(const std::string &path, uint8_t *destination, size_t size)
{
    if (!File::exist(path))
    {
        return -1;
    }
    File opened_file(path);
    if (opened_file.get_size() != size)
    {
        std::cout << "ERROR: unable to load file: " << path << ". File is corrypted. "
                  << "Expected file size: " << size << ". Existed file size: " << opened_file.get_size() << std::endl;
        return -1;
    }
    std::copy(opened_file.get_ptr(), opened_file.get_ptr() + size, destination);
    return 0;
}

int save_data_to_file(const std::string &path, uint8_t *source, size_t size)
{
    File savefile(path, source, size);
    return 0;
}