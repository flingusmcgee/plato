#pragma once
#include "Game.h"
#include "Input.h"

class Camera {
    public:
        float x, y;
        int dir;
        float tilex, tiley;
        float playerx, playery;

        Coordinate updateCamera(Game& game, Input& input, SDL_FRect& player);
};

float normalize(SDL_FRect rect, bool isY);
SDL_FRect offset(SDL_FRect rect, Coordinate origin);
