#include "Render.h"

/* Render visible tiles and map-bound entities using tileset/spriteset info and origin point */
std::vector<Entity> Render::renderMap(Game& game, SDL_Renderer *renderer, SDL_FRect tile, std::vector<SDL_Texture *>& tileList, SDL_FRect npc, std::vector<SDL_Texture *>& npcList, float origin) {
    renderedTiles = 0;
    entityOrder = { };
    for (int i = 0; i < game.MAPHEIGHT; ++i) {
        for (int j = 0; j < game.MAPWIDTH; ++j) {
            if (tile.x > -game.TILEWIDTH && tile.x < game.SCREENWIDTH && tile.y > -game.TILEHEIGHT && tile.y < game.SCREENHEIGHT) {
                SDL_RenderTexture(renderer, tileList.at(game.MAP[i][j]), NULL, &tile);
                ++renderedTiles;
                /* Queue entity processing if applicable */
                npc.x = tile.x;
                npc.y = tile.y;
                if (game.NPC[i][j] > 0) {
                    entityOrder = orderEntity(game, game.NPCPATHS[game.NPC[i][j]], npcList.at(game.NPC[i][j]), npc);
                }
            }
            tile.x += game.TILEWIDTH;
        }
        tile.x = origin;
        tile.y += game.TILEHEIGHT;
    }
    return entityOrder;
}

/* Aligns entities to a pseudo z-index in preparation of rendering */
std::vector<Entity> Render::orderEntity(Game& game, std::string_view name, SDL_Texture *texture, SDL_FRect rect) {
    if (entityOrder.empty()) {
        entityOrder.push_back({name, texture, rect});
    }
    else {
        for (auto it = entityOrder.begin(); it != entityOrder.end(); ++it) {
            if (rect.y - (game.TILEHEIGHT - rect.h) <= (*it).rect.y) {
                entityOrder.insert(it, {name, texture, rect});
                break;
            }
        }
        if (rect.y - (game.TILEHEIGHT - rect.h) > entityOrder.back().rect.y) {
            entityOrder.push_back({name, texture, rect});
        }
    }
    return entityOrder;
}
