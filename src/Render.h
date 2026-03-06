#pragma once
#include "Game.h"
#include "EntityManager.h"

class Render {
    public:
        int renderMap(SDL_Renderer *renderer, Game& game, std::vector<Entity>& order, SDL_FRect tile, SDL_FRect npc, float origin);
        int orderEntity(std::vector<Entity>& order, int type, SDL_Texture *texture, SDL_FRect rect);
};
