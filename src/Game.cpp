#include "Game.h"

/* Initialize game data using reader.cpp */
void Game::loadGameData(const Game& external) {
    /* Paths */
    SPRITEPATHS  = external.SPRITEPATHS;
    NPCPATHS     = external.NPCPATHS;
    TILEPATHS    = external.TILEPATHS;
    ICONPATHS    = external.ICONPATHS;
    AUDIOPATHS   = external.AUDIOPATHS;
    /* Screen */
    SCREENWIDTH  = external.SCREENWIDTH;
    SCREENHEIGHT = external.SCREENHEIGHT;
    FONTPATH     = external.FONTPATH;
    FONTSIZE     = external.FONTSIZE;
    FONTSPEED    = external.FONTSPEED;
    /* Player */
    SPEED        = external.SPEED;
    SPRITEWIDTH  = external.SPRITEWIDTH;
    SPRITEHEIGHT = external.SPRITEHEIGHT;
    RANGE        = external.RANGE;
    BREADTH      = external.BREADTH;
    /* Map */
    TILEWIDTH    = external.TILEWIDTH;
    TILEHEIGHT   = external.TILEHEIGHT;
    MAPWIDTH     = external.MAPWIDTH;
    MAPHEIGHT    = external.MAPHEIGHT;
    MAPSET       = external.MAPSET;
    MAP          = external.MAP;
    NPCSET       = external.NPCSET;
    NPC          = external.NPC;
    DIALOGUE     = external.DIALOGUE;
}

float Game::getDistance(SDL_FRect self, SDL_FRect target) {
    const float xdist = normalize(self, 0) - normalize(target, 0);
    const float ydist = normalize(self, 1) - normalize(target, 1);
    return SDL_sqrtf(xdist * xdist + ydist * ydist);
}

/* Finds the closest entities within a rectangle above and below the player */
int Game::getClosestTarget(std::vector<Entity>& order, int playeridx, int dir, float width, float height) {
    above = { };
    below = { };

    float xdist;
    float closestdist;
    int closest = -1;

    for (int i = playeridx - 1; i >= 0; --i) {
        if (normalize(order[i].rect, 1) - normalize(order[playeridx].rect, 1) <= -height) {
            break;
        }
        xdist = normalize(order[i].rect, 0) - normalize(order[playeridx].rect, 0);
        if (SDL_fabsf(xdist) <= width && (xdist * (dir - 180) < 0 || dir % 180 == 0)) {
            above.push_back(i);
            if (dir != 180 && (getDistance(order[i].rect, order[playeridx].rect) < closestdist || closest == -1)) {
                closest = i;
                closestdist = getDistance(order[i].rect, order[playeridx].rect);
            }
        }
    }
    for (int i = playeridx + 1; i <= order.size() - 1; ++i) {
        if (normalize(order[i].rect, 1) - normalize(order[playeridx].rect, 1) >= height) {
            break;
        }
        xdist = normalize(order[i].rect, 0) - normalize(order[playeridx].rect, 0);
        if (SDL_fabsf(xdist) <= width && (xdist * (dir - 180) < 0 || dir % 180 == 0)) {
            below.push_back(i);
            if (dir != 0 && (getDistance(order[i].rect, order[playeridx].rect) < closestdist || closest == -1)) {
                closest = i;
                closestdist = getDistance(order[i].rect, order[playeridx].rect);
            }
        }
    }

    return closest;
}

/* SDL_Render uses the top-left corner of an entity, but game logic considers entities at their middle-bottom */
float normalize(SDL_FRect rect, bool isY) {
    return (isY) ? rect.y + rect.h : rect.x + rect.w / 2;
}
