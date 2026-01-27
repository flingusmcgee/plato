#pragma once
#include "Game.h"

class Audio {
    public: 
        SDL_AudioStream *stream;
        SDL_AudioSpec spec;
        Uint32 len;

        std::vector<Uint8*> audioList;

        Uint8* loadAudio(std::string path);
        void createStream();
        void addAudio(Uint8 *audio);
};
