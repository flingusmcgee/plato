#pragma once
#include "Game.h"
#include "Camera.h"

struct Entity {
    int type;
    SDL_Texture *texture;
    SDL_FRect rect;
};

class EntityManager {
    private:
        std::vector<int> above;
        std::vector<int> below;

    public:
        int getClosestTarget(std::vector<Entity>& order, int playeridx, int dir, float width, float height);
        float getDistance(SDL_FRect self, SDL_FRect target);
        float getDirection(SDL_FRect self, SDL_FRect target, Coordinate origin);
        SDL_FRect move(SDL_FRect self, float angle, float speed);
};
