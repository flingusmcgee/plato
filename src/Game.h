#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <string_view>
#include <vector>
#include <map>

struct Coordinate {
    float x;
    float y;
};

struct Text {
    std::vector<SDL_Texture *> texture;
    std::vector<std::string> data;
    std::vector<float> w;
    float h;
    int reftick;
};

struct EntityTemplate {
    int type;
    std::string group;
    std::vector<std::string> texturePaths;
    std::vector<SDL_Texture *> textures;
    SDL_FRect rect;
    SDL_FRect hitbox;
    float speed;
    int range;
    int breadth;
};

class Game {
    public:
        /* Initial game variables */
        /* Paths */
        std::vector<std::string> TILEPATHS;
        std::vector<std::string> ICONPATHS;
        std::vector<std::string> AUDIOPATHS;
        /* Screen */
        int SCREENWIDTH;
        int SCREENHEIGHT;
        std::string FONTPATH;
        int FONTSIZE;
        int FONTSPEED;
        /* Entities */
        std::map<std::string, EntityTemplate> ENTITYTEMPLATES;
        /* Map */
        int TILEWIDTH;
        int TILEHEIGHT;
        int MAPWIDTH;
        int MAPHEIGHT;
        std::vector<std::string> MAPSET;
        std::vector<std::vector<int>> MAP;
        std::vector<std::string> NPCSET;
        std::vector<std::vector<int>> NPC;
        std::vector<std::vector<std::string>> DIALOGUE;

        /* Gameloop variables */
        std::vector<SDL_Texture *> mapTileList;
        std::vector<SDL_Texture *> iconList;

        int ticks;
        bool interacting;

        void loadGameData(const Game& external);
};

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
