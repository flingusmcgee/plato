#pragma once
#include "../main.h"

class Camera {
    public:
        float x;
        float y;
        int dir;
        int tilex, tiley;
        int playerx, playery;

        void updateCamera(GameData& game, SDL_FRect& player, SDL_FRect& tile);
};
