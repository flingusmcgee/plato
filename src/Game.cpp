#include "Game.h"

/* Initialize game data using reader.cpp */
void Game::loadGameData(const Game& external) {
    SCREENWIDTH  = external.SCREENWIDTH;
    SCREENHEIGHT = external.SCREENHEIGHT;
    SPEED        = external.SPEED;
    SPRITEPATHS  = external.SPRITEPATHS;
    NPCPATHS     = external.NPCPATHS;
    TILEPATHS    = external.TILEPATHS;
    ICONPATHS    = external.ICONPATHS;
    SPRITEWIDTH  = external.SPRITEWIDTH;
    SPRITEHEIGHT = external.SPRITEHEIGHT;
    TILEWIDTH    = external.TILEWIDTH;
    TILEHEIGHT   = external.TILEHEIGHT;
    MAPWIDTH     = external.MAPWIDTH;
    MAPHEIGHT    = external.MAPHEIGHT;
    MAP          = external.MAP;
    NPC          = external.NPC;
}

float Game::getDistance(SDL_FRect self, SDL_FRect target) {
    const float xdist = normalize(self, 0) - normalize(target, 0);
    const float ydist = normalize(self, 1) - normalize(target, 1);
    return SDL_sqrtf(xdist * xdist + ydist * ydist);
}

/* Finds the closest entities within a rectangle above and below the player */
int Game::getClosestTarget(const Game& game, int playeridx, int dir, float width, float height) {
    above = { };
    below = { };

    for (int i = playeridx; i >= 0; --i) {
        if (normalize(game.entityOrder[i].rect, 1) - normalize(game.entityOrder[playeridx].rect, 1) <= -height) {
            break;
        }
        if (SDL_fabsf(normalize(game.entityOrder[i].rect, 0) - normalize(game.entityOrder[playeridx].rect, 0)) <= width && i != playeridx) {
            above.push_back(i);
        }
    }
    for (int i = playeridx; i <= game.entityOrder.size() - 1; ++i) {
        if (normalize(game.entityOrder[i].rect, 1) - normalize(game.entityOrder[playeridx].rect, 1) >= height) {
            break;
        }
        if (SDL_fabsf(normalize(game.entityOrder[i].rect, 0) - normalize(game.entityOrder[playeridx].rect, 0)) <= width && i != playeridx) {
            below.push_back(i);
        }
    }

    std::vector<int> closest;
    if (dir != 180) {
        for (int i : above) {
            closest.push_back(i);
        }
    }
    if (dir != 0) {
        for (int i : below) {
            closest.push_back(i);
        }
    }

    return (closest.empty()) ? -1 : closest[0];
}

/* SDL_Render uses the top-left corner of an entity, but game logic considers entities at their middle-bottom */
float normalize(SDL_FRect rect, bool isY) {
    return (isY) ? rect.y + rect.h : rect.x + rect.w / 2;
}
