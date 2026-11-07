#pragma once
#include "Utility.h"

class Entity;
struct EntityTemplate;

class Game {
    public:
        /* Initial game variables */
        /* Paths */
        std::vector<std::string> TILEPATHS;
        std::vector<std::string> ICONPATHS;
        std::vector<std::string> AUDIOPATHS;
        /* Screen */
        float SCREENWIDTH;
        float SCREENHEIGHT;
        std::string FONTPATH;
        float FONTSIZE;
        float FONTSPEED;
        float ANIMATIONSPEED;
        bool SHOWBOXES;
        /* Entities */
        std::map<std::string, EntityTemplate> ENTITYTEMPLATES;
        /* Map */
        int MAXENTITIES;
        int MAPWIDTH;
        int MAPHEIGHT;
        int TILEWIDTH;
        int TILEHEIGHT;
        std::vector<std::string> MAPSET;
        std::vector<std::vector<int>> MAP;
        std::vector<std::string> NPCSET;
        std::vector<std::vector<int>> NPC;
        std::vector<std::vector<std::string>> DIALOGUE;

        /* Gameloop variables */
        std::vector<SDL_Texture *> mapTileList;
        std::vector<SDL_Texture *> iconList;
        Uint64 ticks;
        float delta;
        bool interacting;

        std::map<std::string, Entity> existingEntities;
        std::vector<Entity*> entityOrder;

        void loadGameData(const Game& external);
        bool isInView(SDL_FRect rect) const;
        void highlight(SDL_Renderer *renderer, SDL_FRect drect);
};
