#pragma once
#include "Game.h"
#include "Asset.h"

class Interface {
    public:
        TTF_Font *font;
        Text text;

        void createTextBox(SDL_Renderer *renderer, Game& game, int closest);
        void renderTextBox(SDL_Renderer *renderer, const Game& game, float height, float margin);
        void clearText();
};
