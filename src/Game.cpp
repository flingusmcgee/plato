#include "Game.h"

/* Initialize game data using `reader.cpp` */
void Game::loadGameData(const Game& external) {
    /* Paths */
    TILEPATHS    = external.TILEPATHS;
    ICONPATHS    = external.ICONPATHS;
    AUDIOPATHS   = external.AUDIOPATHS;
    /* Screen */
    SCREENWIDTH  = external.SCREENWIDTH;
    SCREENHEIGHT = external.SCREENHEIGHT;
    FONTPATH     = external.FONTPATH;
    FONTSIZE     = external.FONTSIZE;
    FONTSPEED    = external.FONTSPEED;
    ANIMATIONSPEED  = external.ANIMATIONSPEED;
    SHOWBOXES    = external.SHOWBOXES;
    /* Entities */
    ENTITYTEMPLATES = external.ENTITYTEMPLATES;
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

/* Get the coordinates of the top-left corner required for `SDL_Render`. Inverse of `getPhysicsAnchor` */
SDL_FRect getRenderAnchor(SDL_FRect rect) {
    return { rect.x - rect.w / 2, rect.y - rect.h, rect.w, rect.h };
}

/* Get the coordinates of the bottom-middle required for physics. Inverse of `getRenderAnchor` */
SDL_FRect getPhysicsAnchor(SDL_FRect rect) {
    return { rect.x + rect.w / 2, rect.y + rect.h, rect.w, rect.h };
}

/* Translates a rect's x and y coordinates */
SDL_FRect translate(SDL_FRect rect, Vector offset) {
    return { rect.x + offset.x, rect.y + offset.y, rect.w, rect.h };
}

/* Converts a movement vector into a translatable offset value */
Vector getVector(float angle, float magnitude) {
    return { SDL_cosf(angle) * magnitude, SDL_sinf(angle) * magnitude };
}

/* Accepts the render rectangle `drect` of the target */
bool isInView(SDL_FRect rect, const Game& game) {
    return rect.x > -rect.w && rect.x < game.SCREENWIDTH && rect.y > -rect.h && rect.y < game.SCREENHEIGHT;
}
