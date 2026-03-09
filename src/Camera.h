#pragma once
#include "Game.h"
#include "Input.h"

class Camera {
    public:
        Coordinate origin;
        float x, y;
        int dir;
        float tilex, tiley;
        float playerx, playery;

        void updateCamera(Game& game, Input& input, SDL_FRect& prect);
        SDL_FRect offset(SDL_FRect rect);
};

SDL_FRect getPhysicsAnchor(SDL_FRect rect);
SDL_FRect getRenderAnchor(SDL_FRect rect);
SDL_FRect translate(SDL_FRect rect, Coordinate offset);
