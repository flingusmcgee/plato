#pragma once
#include "Game.h"
#include "Entity.h"

class Render {
    public:
        int renderMap(SDL_Renderer *renderer, const Game& game, std::vector<Entity>& order, Vector origin);
        int orderEntity(std::vector<Entity>& order, Entity& entity);
        void renderBigPoint(SDL_Renderer *renderer, float x, float y, float size);
        void renderCircle(SDL_Renderer *renderer, float x, float y, float radius);
};
