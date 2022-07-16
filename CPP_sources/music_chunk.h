#pragma once
#include <string>

#include <SDL_mixer.h>

#include "wav.h"

class Music_chunk
{
public:
    Music_chunk();
    Music_chunk(const std::string &path);
    Music_chunk(const WAV &wav_file);
    Music_chunk(const Music_chunk &chunk);
    ~Music_chunk();

    Music_chunk &operator=(const Music_chunk &chunk) = delete;

    int load_chunk(const WAV &wav_file);
    Mix_Music *get_chunk();
    const Mix_Music *get_chunk() const;
    bool is_allocated() const;

private:
    int destroy_chunk();
    Mix_Music *m_chunk_ptr = nullptr;
    WAV m_wav;
};
