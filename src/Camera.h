#pragma once
#include "Utility.h"

class Game;

class Camera {
    public:
        Vector origin;
        float tilex, tiley;
        float playerx, playery;

        Vector shakeOffset;
        int reftick;

        void updateCamera(const Game& game, const SDL_FRect& prect);
        SDL_FRect offsetToCamera(SDL_FRect rect) const;
        void shakeCamera(int strength, int tick);
};

