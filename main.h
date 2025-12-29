#include <SDL3/SDL.h>
#include <string>
#include <vector>

static std::vector<SDL_Texture *> playerSpriteList = { };
static std::vector<SDL_Texture *> npcSpriteList    = { };
static std::vector<SDL_Texture *> mapTileList      = { };

const static int SCREENWIDTH = 640;
const static int SCREENHEIGHT = 480;

const static float SPEED = 1.0f;

const static std::vector<std::string> SPRITEPATHS = {
    "assets/timmy_n.png",
    "assets/timmy_e.png",
    "assets/timmy_s.png",
    "assets/timmy_w.png",
};
const static std::vector<std::string> NPCPATHS = {
    "assets/lollipop.png",
};
const static std::vector<std::string> TILEPATHS = {
    "assets/blank.png",
    "assets/grass.png",
    "assets/stone.png",
    "assets/dirt.png",
    "assets/sand.png",
    "assets/water.png",
};

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

const static int SPRITEWIDTH = 128;
const static int SPRITEHEIGHT = 128;
const static int TILEWIDTH = 256;
const static int TILEHEIGHT = 256;
const static int MAPWIDTH = 5;
const static int MAPHEIGHT = 5;
const static int MAP[MAPWIDTH][MAPHEIGHT] = {
    { BLANK, BLANK, BLANK, BLANK, BLANK },
    { BLANK, GRASS, GRASS, GRASS, STONE },
    { BLANK, GRASS, GRASS, GRASS, STONE },
    { BLANK, GRASS, GRASS, GRASS, STONE },
    { BLANK, WATER, WATER, WATER, WATER },
};
const static int NPC[MAPWIDTH][MAPHEIGHT] = {
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, LOLLI, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, LOLLI, EMPTY},
};

struct Entity {
    std::string name;
    SDL_Texture **texture;
    SDL_FRect rect;
};

static std::vector<Entity> entityOrder = { };

static SDL_AppResult loadTexture(std::string filePath, std::vector<SDL_Texture *> *destination);
static void orderEntity(std::string name, SDL_Texture **texture, SDL_FRect rect);
