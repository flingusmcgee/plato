#pragma once
#include "Game.h"

static SDL_Gamepad *gamepad;

class Camera {
    public:
        float x, y;
        int dir;
        float tilex, tiley;
        float playerx, playery;

        void updateCamera(Game& game, SDL_Gamepad *gamepad, SDL_FRect& player, SDL_FRect& tile);
};
