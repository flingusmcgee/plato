#include "Render.h"
#include "Game.h"
#include "Entity.h"
#include "Camera.h"

/* Render visible tiles and initialize map-bound entities using tileset/spriteset info. Returns the number of rendered tiles */
int Render::renderMap(SDL_Renderer *renderer, Game& game, const Camera& camera) {
    SDL_FRect tile = { 0, 0, (float) game.TILEWIDTH, (float) game.TILEHEIGHT };
    int renderedTiles = 0;
    for (int i = 0; i < game.MAPHEIGHT; ++i) {
        for (int j = 0; j < game.MAPWIDTH; ++j) {
            /* Render map tiles */
            SDL_FRect tileOffset = camera.offsetToCamera(tile);
            if (game.isInView(tileOffset)) {
                SDL_RenderTexture(renderer, game.mapTileList.at(game.MAP[i][j]), NULL, &tileOffset);
                ++renderedTiles;
            }

            /* Queue entity processing if applicable */
            Vector entitypos;
            entitypos.x = getPhysicsAnchor(tile).x;
            entitypos.y = getPhysicsAnchor(tile).y;
            if (game.NPC[i][j] > 0) {
                std::string id = std::format("{}{}{}", game.NPCSET[game.NPC[i][j]], entitypos.x, entitypos.y);
                if (game.existingEntities.size() < game.MAXENTITIES) {
                    if (!game.existingEntities.contains(id)) {
                        Entity entity(game.ENTITYTEMPLATES.at(game.NPCSET[game.NPC[i][j]]), entitypos);
                        entity.updateEntity(camera);
                        game.existingEntities[id] = entity;
                    }
                }
            }

            tile.x += game.TILEWIDTH;
        }
        tile.x = 0;
        tile.y += game.TILEHEIGHT;
    }

    return renderedTiles;
}

/* Aligns entities to a pseudo z-index in preparation of rendering. Returns the index of the newly added entity */
int Render::orderEntity(std::vector<Entity*>& order, Entity* entity) {
    int idx = 0;
    if (order.empty()) {
        order.push_back(entity);
    }
    else {
        for (auto it = order.begin(); it != order.end(); ++it) {
            if (entity->prect.y <= (*it)->prect.y) {
                order.insert(it, entity);
                break;
            }
            ++idx;
        }
        if (entity->prect.y > order.back()->prect.y) {
            order.push_back(entity);
        }
    }

    return idx;
}

/* Intended to replace `SDL_RenderPoint`. Debug purposes only */
void Render::renderBigPoint(SDL_Renderer *renderer, float x, float y, float size) {
    SDL_FRect rect;
    rect.x = x - size / 2;
    rect.y = y - size / 2;
    rect.w = size;
    rect.h = size;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
}

void Render::renderCircle(SDL_Renderer *renderer, float x, float y, float radius) {
    for (int i = 0; i < 16; ++i) {
        renderBigPoint(renderer, x + SDL_sinf(i * SDL_PI_F / 8) * radius, y + SDL_cosf(i * SDL_PI_F / 8) * radius, 2);
    }
}
