#pragma once
#include "Game.h"
#include "Entity.h"
#include "Asset.h"

class Interface {
    private:
        int line;
        int page;
        float progress;

    public:
        TTF_Font *font;
        Text text;

        void createTextBox(SDL_Renderer *renderer, Game& game, Entity entity, int maxWidth);
        void renderTextBox(SDL_Renderer *renderer, const Game& game, float height, float margin);
        void clearText();
};
