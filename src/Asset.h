#pragma once
#include "../main.h"

class Asset {
    public:
        SDL_Texture* loadTexture(SDL_Renderer *renderer, std::string filePath);
        std::vector<SDL_Texture *> loadTexturesInto(SDL_Renderer *renderer, std::vector<std::string> pathList);
        TTF_Font* loadFont(std::string fontPath, float fontSize);
        Text loadFontTexture(SDL_Renderer *renderer, TTF_Font* font, std::string text, SDL_Color color);
};
