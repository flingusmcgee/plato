#pragma once
#include "Game.h"
#include "Camera.h"

class Entity {
    private:
        std::vector<int> above;
        std::vector<int> below;

    public:
        Entity(int type, std::string group, SDL_Texture *texture, SDL_FRect rect) {
            this->type = type;
            this->group = group;
            this->texture = texture;
            this->prect = rect;
            this->drect = rect;
        }

        int type;
        std::string group;
        SDL_Texture *texture;
        SDL_FRect prect; /* Physics position */
        SDL_FRect drect; /* Rendering position */

        int getClosestTarget(std::vector<Entity>& order, std::string group, int playeridx, int dir, float width, float height);
        float getDistance(SDL_FRect target);
        float getDirection(SDL_FRect target);
        SDL_FRect move(float angle, float speed);
};
