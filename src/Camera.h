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

        Coordinate shakeOffset;
        int reftick;

        void updateCamera(Game& game, Input& input, SDL_FRect& prect, float speed);
        SDL_FRect offset(SDL_FRect rect);
        void shakeCamera(int strength, int tick);
};

SDL_FRect getRenderAnchor(SDL_FRect rect);
SDL_FRect getPhysicsAnchor(SDL_FRect rect);
SDL_FRect translate(SDL_FRect rect, Coordinate offset);
Coordinate translate(Coordinate point, Coordinate offset);
Coordinate getMoveOffset(float angle, float speed);
bool isInView(SDL_FRect rect, Game& game);
