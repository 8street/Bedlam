#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

class File
{
public:
    File();
    File(const std::string &path);
    File(const std::string &path, const std::vector<uint8_t> &data);
    File(const std::string &path, const uint8_t *data, size_t size);

    int save(const std::string &path, const std::vector<uint8_t> &data);
    int load(const std::string &path);
    static bool exist(const std::string &path);
    const uint8_t *get_ptr() const;
    uint8_t *get_ptr();
    std::string get_full_path() const;
    std::string get_directory() const;
    std::string get_filename() const;
    std::string get_extension() const;
    uint16_t get_file_header() const;
    size_t get_size() const;

private:
    int load_from_disk();
    int save_to_disk();
    std::filesystem::path m_path;
    std::vector<uint8_t> m_data;
};
