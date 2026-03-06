#include "Audio.h"

Uint8* Audio::loadAudio(std::string path) {
    Uint8 *audio;
    if (!SDL_LoadWAV(path.c_str(), &spec, &audio, &len)) {
        SDL_Log("WAVE load error: %s", SDL_GetError());
    }
    return audio;
}

void Audio::createStream() {
    spec = { SDL_AUDIO_S16LE, 2, 44100 };
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    SDL_ResumeAudioStreamDevice(stream);
}

void Audio::addAudio(Uint8 *audio) {
    if (!SDL_PutAudioStreamData(stream, audio, len)) {
        SDL_Log("Audio stream add error: %s", SDL_GetError());
    }
}
