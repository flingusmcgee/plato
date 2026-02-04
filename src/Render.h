#pragma once
#include "Game.h"

class Render {
    public:
        int renderMap(SDL_Renderer *renderer, const Game& game, std::vector<Entity>& order, SDL_FRect tile, SDL_FRect npc, float origin);
        int orderEntity(const Game& game, std::vector<Entity>& order, int type, SDL_Texture *texture, SDL_FRect rect);
};
