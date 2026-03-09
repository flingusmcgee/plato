#include "Render.h"

/* Render visible tiles and map-bound entities using tileset/spriteset info and origin point. Returns the number of rendered tiles */
int Render::renderMap(SDL_Renderer *renderer, Game& game, std::vector<Entity>& order, SDL_FRect tile, SDL_FRect npc, Coordinate origin) {
    int renderedTiles = 0;
    order = { };
    for (int i = 0; i < game.MAPHEIGHT; ++i) {
        for (int j = 0; j < game.MAPWIDTH; ++j) {
            if (tile.x > -game.TILEWIDTH && tile.x < game.SCREENWIDTH && tile.y > -game.TILEHEIGHT && tile.y < game.SCREENHEIGHT) {
                SDL_RenderTexture(renderer, game.mapTileList.at(game.MAP[i][j]), NULL, &tile);
                ++renderedTiles;
                /* Queue entity processing if applicable */
                npc.x = tile.x + tile.w / 2 - origin.x;
                npc.y = tile.y + tile.h - origin.y;
                if (game.NPC[i][j] > 0) {
                    Entity entity(game.NPC[i][j], "npc", game.npcSpriteList.at(game.NPC[i][j]), npc);
                    orderEntity(order, entity);
                }
            }
            tile.x += game.TILEWIDTH;
        }
        tile.x = origin.x;
        tile.y += game.TILEHEIGHT;
    }

    return renderedTiles;
}

/* Aligns entities to a pseudo z-index in preparation of rendering. Returns the index of the newly added entity */
int Render::orderEntity(std::vector<Entity>& order, Entity& entity) {
    int idx = 0;
    if (order.empty()) {
        order.push_back(entity);
    }
    else {
        for (auto it = order.begin(); it != order.end(); ++it) {
            if (entity.prect.y <= (*it).prect.y) {
                order.insert(it, entity);
                break;
            }
            ++idx;
        }
        if (entity.prect.y > order.back().prect.y) {
            order.push_back(entity);
        }
    }

    return idx;
}

/* Intended to replace SDL_RenderPoint. Debug purposes only */
void Render::renderBigPoint(SDL_Renderer *renderer, float x, float y) {
    SDL_FRect rect;
    rect.x = x - 5;
    rect.y = y - 5;
    rect.w = 10;
    rect.h = 10;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
}
