#include "Game.h"
#include "Entity.h"

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
    MAXENTITIES  = external.MAXENTITIES;
    MAPWIDTH     = external.MAPWIDTH;
    MAPHEIGHT    = external.MAPHEIGHT;
    TILEWIDTH    = external.TILEWIDTH;
    TILEHEIGHT   = external.TILEHEIGHT;
    MAPSET       = external.MAPSET;
    MAP          = external.MAP;
    NPCSET       = external.NPCSET;
    NPC          = external.NPC;
    DIALOGUE     = external.DIALOGUE;
}

/* Accepts the render rectangle `drect` of the target */
bool Game::isInView(SDL_FRect rect) const {
    return rect.x > -rect.w && rect.x < SCREENWIDTH && rect.y > -rect.h && rect.y < SCREENHEIGHT;
}

void Game::highlight(SDL_Renderer *renderer, SDL_FRect drect) {
    const float DISTANCE = 10;
    SDL_FRect highlighter = drect;
    highlighter.y += SDL_sinf((float) ticks / NSCOEFFICIENT * ANIMATIONSPEED) * DISTANCE;
    SDL_RenderTexture(renderer, iconList[0], NULL, &highlighter);
}