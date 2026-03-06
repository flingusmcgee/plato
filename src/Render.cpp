#include "Render.h"

/* Render visible tiles and map-bound entities using tileset/spriteset info and origin point. Returns the number of rendered tiles */
int Render::renderMap(SDL_Renderer *renderer, Game& game, std::vector<Entity>& order, SDL_FRect tile, SDL_FRect npc, float origin) {
    int renderedTiles = 0;
    order = { };
    for (int i = 0; i < game.MAPHEIGHT; ++i) {
        for (int j = 0; j < game.MAPWIDTH; ++j) {
            if (tile.x > -game.TILEWIDTH && tile.x < game.SCREENWIDTH && tile.y > -game.TILEHEIGHT && tile.y < game.SCREENHEIGHT) {
                SDL_RenderTexture(renderer, game.mapTileList.at(game.MAP[i][j]), NULL, &tile);
                ++renderedTiles;
                /* Queue entity processing if applicable */
                npc.x = tile.x;
                npc.y = tile.y;
                if (game.NPC[i][j] > 0) {
                    orderEntity(order, game.NPC[i][j], game.npcSpriteList.at(game.NPC[i][j]), npc);
                }
            }
            tile.x += game.TILEWIDTH;
        }
        tile.x = origin;
        tile.y += game.TILEHEIGHT;
    }

    return renderedTiles;
}

/* Aligns entities to a pseudo z-index in preparation of rendering. Returns the index of the newly added entity */
int Render::orderEntity(std::vector<Entity>& order, int type, SDL_Texture *texture, SDL_FRect rect) {
    int idx = 0;
    if (order.empty()) {
        order.push_back({type, texture, rect});
    }
    else {
        for (auto it = order.begin(); it != order.end(); ++it) {
            if (normalize(rect, 1) <= normalize((*it).rect, 1)) {
                order.insert(it, {type, texture, rect});
                break;
            }
            ++idx;
        }
        if (normalize(rect, 1) > normalize(order.back().rect, 1)) {
            order.push_back({type, texture, rect});
        }
    }

    return idx;
}
