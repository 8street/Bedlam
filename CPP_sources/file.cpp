#include <algorithm>
#include <fstream>
#include <iostream>

#include <SDL.h>

#include "file.h"
#include "helper.h"

File::File()
{
}

File::File(const std::string &path)
    : m_path(path)
{
    load_from_disk();
}

File::File(const std::string &path, const std::vector<uint8_t> &data)
{
    save(path, data);
}

int File::save(const std::string &path, const std::vector<uint8_t> &data)
{
    m_path = path;
    m_data.clear();
    m_data.insert(m_data.end(), data.begin(), data.end());
    return save_to_disk();
}

int File::load(const std::string &path)
{
    const std::filesystem::path new_path = path;
    if (new_path == m_path && !m_data.empty())
    {
        return 0;
    }
    m_path = path;
    
    return load_from_disk();
}

bool File::exist(const std::string &path)
{ 
    return std::filesystem::exists(path);
}

std::string File::get_full_path() const
{
    return m_path.string();
}

std::string File::get_directory() const
{
    return m_path.parent_path().string();
}

std::string File::get_filename() const
{
    return m_path.filename().string();
}

std::string File::get_extension() const
{
    return m_path.extension().string();
}

int File::load_from_disk()
{
    std::ifstream file;
    file.open(get_full_path().c_str(), std::ios_base::in | std::ios_base::binary);
    if (!file.is_open())
    {
        file.close();
        std::string path = to_upper(get_full_path());
        m_path.assign(path);
        file.open(get_full_path().c_str(), std::ios_base::in | std::ios_base::binary);
        if (!file.is_open())
        {
            std::string error_str("Unable to open file ");
            error_str += get_full_path();
            std::cout << error_str << std::endl;
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Problem", error_str.c_str(), NULL);
            exit(404);
        }
    }

    // get filesize
    file.seekg(0, std::ios_base::end);
    const std::ifstream::pos_type file_end_pos = file.tellg();
    file.seekg(0, std::ios::beg);
    const size_t file_size = static_cast<size_t>(file_end_pos);

    m_data.clear();
    m_data.resize(file_size);

    file.read(reinterpret_cast<char *>(m_data.data()), static_cast<std::streamsize>(file_size));
    file.close();
    return 0;
}

uint16_t File::get_file_header() const
{
    return *((uint16_t *)m_data.data());
}

const uint8_t *File::get_ptr() const
{
    return m_data.data();
}

uint8_t *File::get_ptr()
{
    return m_data.data();
}

size_t File::get_size() const
{
    return m_data.size();
}

int File::save_to_disk()
{
    std::filesystem::create_directories(m_path.parent_path());
    std::ofstream file(get_full_path().c_str(), std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        std::string error_str("Unable to save file ");
        error_str += get_full_path();
        std::cout << error_str << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Problem", error_str.c_str(), NULL);
        return -1;
    }
    file.write(reinterpret_cast<const char *>(m_data.data()), static_cast<std::streamsize>(get_size()));
    file.close();
    return 0;
}