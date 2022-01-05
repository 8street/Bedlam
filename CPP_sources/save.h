#pragma once
#include <cstdint>
#include <string>

extern "C" int get_saves(uint8_t *save_data);
extern "C" int saves_to_disk(uint8_t *save_data);

extern "C" int get_hiscores(uint8_t *hiscores_data);
extern "C" int hiscores_to_disk(uint8_t *hiscores_data);

extern "C" int saves_exist();
extern "C" int hiscores_exist();

int copy_data_from_file(const std::string &path, uint8_t *destination, size_t size);
int save_data_to_file(const std::string &path, uint8_t *source, size_t size);