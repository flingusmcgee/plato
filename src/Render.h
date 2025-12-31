#pragma once
#include "../main.h"

class Render {
    public:
        int renderedTiles = 0;
        std::vector<Entity> renderMap(GameData& game, SDL_Renderer *renderer, SDL_FRect tile, std::vector<SDL_Texture *>& tileList, SDL_FRect npc, std::vector<SDL_Texture *>& npcList, float origin);
        std::vector<Entity> orderEntity(GameData& game, std::string name, SDL_Texture *texture, SDL_FRect rect);
};
