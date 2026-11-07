#pragma once
#include "Utility.h"

class Audio {
    public: 
        SDL_AudioStream *stream;
        SDL_AudioSpec spec;
        Uint32 len;

        Uint8 *bg;
        int bgtype;

        Uint8* loadAudio(std::string path);
        void createStream();
        void addAudio(Uint8 *audio);
};
