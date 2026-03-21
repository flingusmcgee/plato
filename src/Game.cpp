#include "Game.h"

/* Initialize game data using reader.cpp */
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
