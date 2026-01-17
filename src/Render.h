#pragma once
#include "Game.h"

class Render {
    public:
        int renderedTiles;
        std::vector<Entity> renderMap(Game& game, SDL_Renderer *renderer, SDL_FRect tile, std::vector<SDL_Texture *>& tileList, SDL_FRect npc, std::vector<SDL_Texture *>& npcList, float origin);
        std::vector<Entity> orderEntity(Game& game, std::string_view name, SDL_Texture *texture, SDL_FRect rect);
};
