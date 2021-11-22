#include <iostream>

#include "helper.h"
#include "wav.h"

WAV::WAV(const uint8_t *const raw_ptr, size_t data_size, uint32_t samplerate, uint32_t bits_per_sample, uint32_t num_channels)
{
    load_raw_data(raw_ptr, data_size, samplerate, bits_per_sample, num_channels);
}

WAV::WAV(const File &raw_file, uint32_t samplerate, uint32_t bits_per_sample, uint32_t num_channels)
{
    load_raw_data(raw_file.get_ptr(), raw_file.get_size(), samplerate, bits_per_sample, num_channels);
}

WAV::WAV(const std::string &path)
{
    const File file(path);
    const std::string file_extension = to_lower(file.get_extension());
    if (file_extension == ".raw")
    {
        load_raw_data(file.get_ptr(), file.get_size(), 11025, 8, 1);
    }
    else if (file_extension == ".wav")
    {
        load_wav_data(file.get_ptr(), file.get_size());
    }
    else
    {
        std::cout << "Unknown wave format: " << file.get_full_path() << std::endl;
    }
}

WAV::~WAV()
{
    //destroy_chunk();
}

const uint8_t *WAV::get_data() const
{
    return m_wav_data.data();
}

size_t WAV::get_size() const
{
    return m_wav_data.size();
}

Mix_Chunk* WAV::get_chunk()
{
    return m_wav_chunk;
}

const Mix_Chunk *WAV::get_chunk() const
{
    return m_wav_chunk;
}

int WAV::fill_wav_header(
    WAV_header &header, size_t data_size, uint32_t samplerate, uint32_t bits_per_sample, uint32_t num_channels)
{
    if (samplerate > 384000 || bits_per_sample > 32 || num_channels > 8)
    {
        std::cout << "Invalid wav parameters. Samplerate: " << samplerate;
        std::cout << " Bits per sample: " << bits_per_sample;
        std::cout << " Num channels: " << num_channels << std::endl;
        return -1;
    }
    header.sampleRate = samplerate;
    header.bitsPerSample = bits_per_sample;
    header.numChannels = num_channels;
    header.bytesPerSec = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = header.numChannels * header.bitsPerSample / 8;
    header.subchunk2Size = data_size;
    header.chunkSize = 36 + header.subchunk2Size;
    return 0;
}

int WAV::load_raw_data(
    const uint8_t *const raw_ptr, size_t data_size, uint32_t samplerate, uint32_t bits_per_sample, uint32_t num_channels)
{
    int ret_val = 0;
    if (!raw_ptr || data_size == 0)
    {
        return -1;
    }
    WAV_header header;
    m_wav_data.clear();
    m_wav_data.reserve(sizeof(WAV_header) + data_size);
    ret_val |= fill_wav_header(header, data_size, samplerate, bits_per_sample, num_channels);
    const uint8_t *const header_ptr = reinterpret_cast<uint8_t *>(&header);
    m_wav_data.insert(m_wav_data.end(), header_ptr, header_ptr + sizeof(WAV_header));
    m_wav_data.insert(m_wav_data.end(), raw_ptr, raw_ptr + data_size);
    ret_val |= load_chunk();
    return ret_val;
}

int WAV::load_wav_data(const uint8_t *const raw_ptr, size_t data_size)
{
    if (!raw_ptr || data_size == 0)
    {
        return -1;
    }
    m_wav_data.clear();
    m_wav_data.reserve(data_size);
    m_wav_data.insert(m_wav_data.end(), raw_ptr, raw_ptr + data_size);
    return load_chunk();
}

int WAV::load_chunk()
{
    destroy_chunk();
    SDL_RWops *io = SDL_RWFromConstMem(m_wav_data.data(), get_size());
    if (!io)
    {
        std::cout << "WAV error. Couldn't load data to SDL_RWops. " << SDL_GetError() << std::endl;
        return -1;
    }
    m_wav_chunk = Mix_LoadWAV_RW(io, false);
    if (SDL_RWclose(io))
    {
        std::cout << "WAV error. Couldn't close SDL_RWops. " << SDL_GetError() << std::endl;
    }
    if (!m_wav_chunk)
    {
        std::cout << "WAV error. Couldn't load data chunk. " << Mix_GetError() << std::endl;
        return -1;
    }
    return 0;
}

int WAV::destroy_chunk()
{
    if (m_wav_chunk)
    {
        Mix_FreeChunk(m_wav_chunk);
        m_wav_chunk = nullptr;
    }
    return 0;
}