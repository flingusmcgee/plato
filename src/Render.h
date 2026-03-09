#pragma once
#include "Game.h"
#include "Entity.h"

class Render {
    public:
        int renderMap(SDL_Renderer *renderer, Game& game, std::vector<Entity>& order, SDL_FRect tile, SDL_FRect npc, Coordinate origin);
        int orderEntity(std::vector<Entity>& order, Entity& entity);
        void renderBigPoint(SDL_Renderer *renderer, float x, float y);
};
