#include <cstdlib>
#include <iostream>

#include "music_chunk.h"

Music_chunk::Music_chunk()
{
}

Music_chunk::Music_chunk(const std::string &path)
{
    WAV wav_file(path);
    load_chunk(wav_file);
}

Music_chunk::Music_chunk(const WAV &wav_file)
{
    load_chunk(m_wav);
}

Music_chunk::Music_chunk(const Music_chunk& chunk)
{
    if (chunk.is_allocated())
    {
        destroy_chunk();
        load_chunk(m_wav);
    }
}

Music_chunk::~Music_chunk()
{
    destroy_chunk();
}

int Music_chunk::load_chunk(const WAV &wav_file)
{
    destroy_chunk();
    m_wav = wav_file;
    SDL_RWops *io = SDL_RWFromConstMem(wav_file.get_data(), wav_file.get_size());
    if (!io)
    {
        std::cout << "Music chunk error. Couldn't load data to SDL_RWops. " << SDL_GetError() << std::endl;
        return -1;
    }
    m_chunk_ptr = Mix_LoadMUSType_RW(io, MUS_WAV, false);
    //m_chunk_ptr = Mix_LoadMUS_RW(io, false);
    if (SDL_RWclose(io))
    {
        std::cout << "Music chunk error. Couldn't close SDL_RWops. " << SDL_GetError() << std::endl;
    }
    if (!m_chunk_ptr)
    {
        std::cout << "Music chunk error. Couldn't load data chunk. " << Mix_GetError() << std::endl;
        return -1;
    }
    return 0;
}

Mix_Music *Music_chunk::get_chunk()
{
    return m_chunk_ptr;
}

const Mix_Music *Music_chunk::get_chunk() const
{
    return m_chunk_ptr;
}

bool Music_chunk::is_allocated() const
{
    return static_cast<bool>(m_chunk_ptr);
}

int Music_chunk::destroy_chunk()
{
    m_wav.~WAV();
    if (!m_chunk_ptr)
    {
        return 0;
    }
    Mix_FreeMusic(m_chunk_ptr);
    m_chunk_ptr = nullptr;
    return 0;
}