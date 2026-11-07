#pragma once
#include "Utility.h"

class Game;
class Entity;
class Camera;

class Render {
    public:
        int renderMap(SDL_Renderer *renderer, Game& game, const Camera& camera);
        int orderEntity(std::vector<Entity*>& order, Entity* entity);
        void renderBigPoint(SDL_Renderer *renderer, float x, float y, float size);
        void renderCircle(SDL_Renderer *renderer, float x, float y, float radius);
};
