#pragma once
#include "Game.h"
#include "Camera.h"

class Entity {
    private:
        std::vector<int> above;
        std::vector<int> below;

    public:
        Entity(EntityTemplate entityTemplate) {
            type     = entityTemplate.type;
            group    = entityTemplate.group;
            textures = entityTemplate.textures;
            prect    = entityTemplate.rect;
            hitbox   = entityTemplate.hitbox;
            speed    = entityTemplate.speed;
            range    = entityTemplate.range;
            breadth  = entityTemplate.breadth;
        }

        int type;
        std::string group;
        int texture = 0;
        std::vector<SDL_Texture *> textures;
        SDL_FRect prect; /* Physics position */
        SDL_FRect drect; /* Rendering position */
        SDL_FRect hitbox;
        float speed;
        int range;   /* Player only */
        int breadth; /* Player only */

        void updateEntity(Coordinate offset, int texture);
        int getClosestTarget(std::vector<Entity>& order, std::string group, int playeridx, int dir, float width, float height);
        float getDistance(SDL_FRect target);
        float getDirection(SDL_FRect target);
        void highlight(SDL_Renderer *renderer, const Game& game);
        bool isColliding(SDL_FRect target);
};
