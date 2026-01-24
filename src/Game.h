#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <string_view>
#include <vector>

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

class Game {
    public:
        /* Initial game variables */
        int SCREENWIDTH;
        int SCREENHEIGHT;
        float SPEED;
        std::vector<std::string> SPRITEPATHS;
        std::vector<std::string> NPCPATHS;
        std::vector<std::string> TILEPATHS;
        std::vector<std::string> ICONPATHS;
        float SPRITEWIDTH;
        float SPRITEHEIGHT;
        float TILEWIDTH;
        float TILEHEIGHT;
        int MAPWIDTH;
        int MAPHEIGHT;
        std::vector<std::vector<int>> MAP;
        std::vector<std::vector<int>> NPC;

        /* Gameloop variables */
        std::vector<SDL_Texture *> playerSpriteList;
        std::vector<SDL_Texture *> npcSpriteList;
        std::vector<SDL_Texture *> mapTileList;
        std::vector<SDL_Texture *> iconList;

        std::vector<Entity> entityOrder;

        Text text;

        std::vector<int> above;
        std::vector<int> below;

        void loadGameData(const Game& external);
        float getDistance(SDL_FRect self, SDL_FRect target);
        int getClosestTarget(const Game& game, int playeridx, int dir, float width, float height);
};

float normalize(SDL_FRect rect, bool isY);
template <typename T>
bool isEmpty(std::vector<std::vector<T>> container, const char *messageTotal, const char *messageComponent);
template <typename T>
bool isEmpty(T container, const char *message);

/**
 * Flexible emptiness check for safe handling - Multi-Dimensional Edition
 * \param container multi-dimensional data structure to be tested
 * \param messageTotal log message on total emptiness
 * \param messageComponent log message on individual emptiness
 */
template <typename T>
bool isEmpty(std::vector<std::vector<T>> container, const char *messageTotal, const char *messageComponent) {
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
bool isEmpty(T container, const char *message) {
    if (container.empty()) {
        SDL_Log("%s", message);
        return true;
    }
    return false;
}
