#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>

static std::vector<SDL_Texture *> playerSpriteList = { };
static std::vector<SDL_Texture *> npcSpriteList    = { };
static std::vector<SDL_Texture *> mapTileList      = { };

enum Tileset {
    BLANK,
    GRASS,
    STONE,
    DIRT,
    SAND,
    WATER,
};

enum NPCset {
    EMPTY = -1,
    LOLLI,
};

struct GameData {
    int SCREENWIDTH;
    int SCREENHEIGHT;
    float SPEED;
    std::vector<std::string> SPRITEPATHS;
    std::vector<std::string> NPCPATHS;
    std::vector<std::string> TILEPATHS;
    int SPRITEWIDTH;
    int SPRITEHEIGHT;
    int TILEWIDTH;
    int TILEHEIGHT;
    int MAPWIDTH;
    int MAPHEIGHT;
    std::vector<std::vector<int>> MAP;
    std::vector<std::vector<int>> NPC;
};

struct Entity {
    std::string name;
    SDL_Texture *texture;
    SDL_FRect rect;
};

static std::vector<Entity> entityOrder = { };

static void loadGameData(GameData external);
