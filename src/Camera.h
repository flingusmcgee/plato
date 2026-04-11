#pragma once
#include "Game.h"
#include "Input.h"

class Camera {
    public:
        Coordinate origin;
        float tilex, tiley;
        float playerx, playery;

        Coordinate shakeOffset;
        int reftick;

        void updateCamera(Game& game, SDL_FRect& prect);
        SDL_FRect offsetToCamera(SDL_FRect rect);
        void shakeCamera(int strength, int tick);
};

SDL_FRect getRenderAnchor(SDL_FRect rect);
SDL_FRect getPhysicsAnchor(SDL_FRect rect);
SDL_FRect translate(SDL_FRect rect, Coordinate offset);
Coordinate translate(Coordinate point, Coordinate offset);
Coordinate getMoveOffset(float angle, float speed);
bool isInView(SDL_FRect rect, Game& game);
