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
    /* Entities */
    ENEMYSPEED   = external.ENEMYSPEED;
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
