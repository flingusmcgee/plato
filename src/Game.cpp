#include "Game.h"

/* Initialize game data using reader.cpp */
void Game::loadGameData(Game external) {
    SCREENWIDTH  = external.SCREENWIDTH;
    SCREENHEIGHT = external.SCREENHEIGHT;
    SPEED        = external.SPEED;
    SPRITEPATHS  = external.SPRITEPATHS;
    NPCPATHS     = external.NPCPATHS;
    TILEPATHS    = external.TILEPATHS;
    SPRITEWIDTH  = external.SPRITEWIDTH;
    SPRITEHEIGHT = external.SPRITEHEIGHT;
    TILEWIDTH    = external.TILEWIDTH;
    TILEHEIGHT   = external.TILEHEIGHT;
    MAPWIDTH     = external.MAPWIDTH;
    MAPHEIGHT    = external.MAPHEIGHT;
    MAP          = external.MAP;
    NPC          = external.NPC;
}
