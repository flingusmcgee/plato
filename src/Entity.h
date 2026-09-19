#pragma once
#include "Game.h"
#include "Camera.h"

/**
 * Represents a movable, interactable, and modifiable object in the world.
 * - Defined entities MUST be listed in `data.json`, and references to their creation MUST use their unique identifier.
 * - Entity attributes are almost entirely declared in `entity.json`, using their unique identifier.
 * - If applicable, the entity can be configured to speak upon interaction in `dialogue.json`, using their unique identifier.
 */
class Entity {
    private:
        std::vector<int> inRange;

    public:
        Entity(Game& game, std::string name);

        int type;
        std::string group;
        int texture = 0;
        std::vector<SDL_Texture *> textures;
        SDL_FRect prect; /* Physics position */
        SDL_FRect drect; /* Rendering position */
        SDL_FRect hitbox;
        int hitboxLayer;
        float speed;
        float dir;
        int range;
        std::map<std::string, int> effects;
        int health;

        void updateEntity(Coordinate offset, int texture, std::vector<Entity>& order);
        void inflictDamage(int amount);
        void inflictEffect(std::string effect, int duration);
        void highlight(SDL_Renderer *renderer, const Game& game);
        Coordinate getMoveControls(Game& game, Input& input);
        int getClosestTarget(std::vector<Entity>& order, std::string group, int playeridx, float entitydir, float range);
        float getDistance(SDL_FRect target);
        float getDirection(SDL_FRect target);
        bool isColliding(SDL_FRect target);
        int getSprite();
};
