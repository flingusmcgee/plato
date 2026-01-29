#pragma once
#include "Game.h"
#include "Input.h"

class Camera {
    public:
        float x, y;
        int dir;
        float tilex, tiley;
        float playerx, playery;

        void updateCamera(Game& game, Input& input, SDL_FRect& player, SDL_FRect& tile);
};
