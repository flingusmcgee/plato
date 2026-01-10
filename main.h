#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <string_view>
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
    EMPTY,
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
    std::string_view name;
    SDL_Texture *texture;
    SDL_FRect rect;
};

struct Text {
    SDL_Texture *texture;
    float w;
    float h;
};

static std::vector<Entity> entityOrder = { };

static void loadGameData(GameData external);
template <typename T>
static bool isEmpty(std::vector<std::vector<T>> container, const char *messageTotal, const char *messageComponent);
template <typename T>
static bool isEmpty(T container, const char *message);
