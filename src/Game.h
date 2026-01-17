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

struct Entity {
    std::string_view name;
    SDL_Texture *texture;
    SDL_FRect rect;
};

static std::vector<Entity> entityOrder;

struct Text {
    SDL_Texture *texture;
    float w;
    float h;
};

class Game {
    public:
        int SCREENWIDTH;
        int SCREENHEIGHT;
        float SPEED;
        std::vector<std::string> SPRITEPATHS;
        std::vector<std::string> NPCPATHS;
        std::vector<std::string> TILEPATHS;
        float SPRITEWIDTH;
        float SPRITEHEIGHT;
        float TILEWIDTH;
        float TILEHEIGHT;
        int MAPWIDTH;
        int MAPHEIGHT;
        std::vector<std::vector<int>> MAP;
        std::vector<std::vector<int>> NPC;

        Text text;

        void loadGameData(Game external);
        template <typename T>
        bool isEmpty(std::vector<std::vector<T>> container, const char *messageTotal, const char *messageComponent);
        template <typename T>
        bool isEmpty(T container, const char *message);
};

/**
 * Flexible emptiness check for safe handling - Multi-Dimensional Edition
 * \param container multi-dimensional data structure to be tested
 * \param messageTotal log message on total emptiness
 * \param messageComponent log message on individual emptiness
 */
template <typename T>
bool Game::isEmpty(std::vector<std::vector<T>> container, const char *messageTotal, const char *messageComponent) {
    bool value = isEmpty(container, messageTotal);
    if (!value) {
        int i = 0;
        for (auto row : container) {
            if (row.empty()) {
                SDL_Log("%s %d", messageComponent, i);
                value = true;
                i++;
            }
        }
    }
    return value;
}
/**
 * Flexible emptiness check for safe handling
 * \param container data structure to be tested
 * \param message log message on total emptiness
 */
template <typename T>
bool Game::isEmpty(T container, const char *message) {
    if (container.empty()) {
        SDL_Log("%s", message);
        return true;
    }
    return false;
}
