#include <algorithm>
#include <cstdlib>
#include <iostream>

#include "sound_chunk.h"

Sound_chunk::Sound_chunk()
{
}

Sound_chunk::Sound_chunk(const std::string &path)
{
    WAV wav_file(path);
    load_chunk(wav_file);
}

Sound_chunk::Sound_chunk(const WAV &wav_file)
{
    load_chunk(wav_file);
}

Sound_chunk::Sound_chunk(const Sound_chunk &chunk)
{
    if (chunk.is_allocated())
    {
        destroy_chunk();
        m_chunk = new Mix_Chunk();
        m_chunk->allocated = chunk.get_chunk()->allocated;
        m_chunk->volume = chunk.get_chunk()->volume;
        m_chunk->alen = chunk.get_chunk()->alen;
        m_chunk->abuf = new uint8_t[m_chunk->alen]();
        std::copy(chunk.get_chunk()->abuf, chunk.get_chunk()->abuf + chunk.get_chunk()->alen, m_chunk->abuf);
        m_used_operator_new = true;
        m_samplerate = chunk.m_samplerate;
        m_format = chunk.m_format;
        m_num_channels = chunk.m_num_channels;
    }
}

Sound_chunk::~Sound_chunk()
{
    destroy_chunk();
}

int Sound_chunk::load_chunk(const WAV &wav_file)
{
    destroy_chunk();
    SDL_RWops *io = SDL_RWFromConstMem(wav_file.get_data(), wav_file.get_size());
    if (!io)
    {
        std::cout << "Sound chunk error. Couldn't load data to SDL_RWops. " << SDL_GetError() << std::endl;
        return -1;
    }
    m_chunk = Mix_LoadWAV_RW(io, false);
    m_used_operator_new = false;
    if (SDL_RWclose(io))
    {
        std::cout << "Sound chunk error. Couldn't close SDL_RWops. " << SDL_GetError() << std::endl;
    }
    if (!m_chunk || !m_chunk->allocated)
    {
        std::cout << "Sound chunk error. Couldn't load data chunk. " << Mix_GetError() << std::endl;
        return -1;
    }
    if (!Mix_QuerySpec(&m_samplerate, &m_format, &m_num_channels))
    {
        std::cout << "Sound chunk error. Couldn't query audio specs. ";
        std::cout << "Audio not opened. " << Mix_GetError() << std::endl;
        return -1;
    }
    return 0;
}

Mix_Chunk *Sound_chunk::get_chunk()
{
    return m_chunk;
}

const Mix_Chunk *Sound_chunk::get_chunk() const
{
    return m_chunk;
}

bool Sound_chunk::is_allocated() const
{
    if (!m_chunk)
    {
        return false;
    }
    return m_chunk->allocated;
}

uint32_t Sound_chunk::get_buf_size() const
{
    if (!m_chunk)
    {
        return 0U;
    }
    return m_chunk->alen;
}

int Sound_chunk::resample(int new_samplerate)
{
    if (new_samplerate == m_samplerate)
    {
        return 0;
    }
    if (new_samplerate > 384000 || new_samplerate <= 0 || !is_allocated())
    {
        return -1;
    }

    //new_samplerate = 2 * abs(44000 - new_samplerate) + new_samplerate;
    const int volume = m_chunk->volume;

    SDL_AudioCVT cvt;
    if(SDL_BuildAudioCVT(
        &cvt, m_format, static_cast<uint8_t>(m_num_channels), m_samplerate, m_format, static_cast<uint8_t>(m_num_channels),
        new_samplerate) == -1)
    {
        std::cout << "Resampler error. SDL_BuildAudioCVT. " << SDL_GetError();
    }
    cvt.len = m_chunk->alen;
    cvt.buf = new uint8_t[cvt.len * cvt.len_mult];
    std::copy(m_chunk->abuf, m_chunk->abuf + m_chunk->alen, cvt.buf);
    if(SDL_ConvertAudio(&cvt))
    {
        std::cout << "Resampler error. SDL_ConvertAudio. " << SDL_GetError();
    }

    destroy_chunk();
    m_chunk = new Mix_Chunk();
    m_chunk->alen = cvt.len_cvt;
    m_chunk->volume = volume;
    m_chunk->abuf = new uint8_t[m_chunk->alen];
    m_chunk->allocated = 1;
    std::copy(cvt.buf, cvt.buf + m_chunk->alen, m_chunk->abuf);
    m_used_operator_new = true;
    m_samplerate = new_samplerate;
    //m_format = m_format;
    //m_num_channels = m_num_channels;

    delete[] cvt.buf;
    return 0;
}

int Sound_chunk::destroy_chunk()
{
    if (!m_chunk)
    {
        return 0;
    }
    if (m_used_operator_new)
    {
        delete[] m_chunk->abuf;
        delete m_chunk;
    }
    else
    {
        Mix_FreeChunk(m_chunk);
    }
    m_chunk = nullptr;
    return 0;
}