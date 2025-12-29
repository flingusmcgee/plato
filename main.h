#pragma once
#include "reader.h"
#include <SDL3/SDL.h>
#include <string>
#include <vector>

static std::vector<SDL_Texture *> playerSpriteList = { };
static std::vector<SDL_Texture *> npcSpriteList    = { };
static std::vector<SDL_Texture *> mapTileList      = { };

static int SCREENWIDTH;
static int SCREENHEIGHT;

static float SPEED;

static std::vector<std::string> SPRITEPATHS = { };
static std::vector<std::string> NPCPATHS = { };
static std::vector<std::string> TILEPATHS = { };

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

static int SPRITEWIDTH;
static int SPRITEHEIGHT;
static int TILEWIDTH;
static int TILEHEIGHT;
static int MAPWIDTH;
static int MAPHEIGHT;
static std::vector<std::vector<int>> MAP = { };
static std::vector<std::vector<int>> NPC = { };

struct Entity {
    std::string name;
    SDL_Texture **texture;
    SDL_FRect rect;
};

static std::vector<Entity> entityOrder = { };

static SDL_AppResult loadTexture(std::string filePath, std::vector<SDL_Texture *> *destination);
static void orderEntity(std::string name, SDL_Texture **texture, SDL_FRect rect);
static void loadGameData(ExternalData external);
