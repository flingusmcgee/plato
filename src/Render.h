#pragma once
#include "Game.h"

class Render {
    public:
        int renderMap(SDL_Renderer *renderer, Game& game, std::vector<Entity>& order, SDL_FRect tile, SDL_FRect npc, float origin);
        int orderEntity(Game& game, std::vector<Entity>& order, std::string_view name, SDL_Texture *texture, SDL_FRect rect);
};
