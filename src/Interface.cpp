#include "Interface.h"

/* Creates text for an entity and toggles interaction boolean */
void Interface::createTextBox(SDL_Renderer *renderer, Game& game, std::vector<Entity>& order, int closest, int maxWidth) {
    if (!game.interacting) {
        game.interacting = true;
        progress = 0;
        line = 0;
    }
    else if (progress < 1) {
        progress = 1;
        line = text.data.size();
    }
    else if (page < game.DIALOGUE[order[closest].type].size() - 1) {
        ++page;
        progress = 0;
        line = 0;
    }
    else {
        game.interacting = false;
        clearText();
    }
    if (game.interacting && closest != -1 && progress < 1) {
        Asset asset;
        std::string textdata = game.DIALOGUE[order[closest].type][page];
        for (SDL_Texture *bits : text.texture) {
            SDL_DestroyTexture(bits);
        }
        text = asset.loadTextTexture(renderer, font, textdata, { 0, 0, 0, 255 }, maxWidth);
        text.reftick = game.ticks;
    }
}

/* Renders text background and animation */
void Interface::renderTextBox(SDL_Renderer *renderer, const Game& game, float height, float margin) {
    SDL_FRect textbox = { margin, game.SCREENHEIGHT - height, game.SCREENWIDTH - margin * 2, height - margin };
    SDL_FRect textsrcrect = { 0, 0, 0, text.h * 10 };
    SDL_FRect textrect = { margin * 2, game.SCREENHEIGHT - height, 0, text.h };

    if (progress < 1) {
        progress = SDL_min((float) (game.ticks - text.reftick) / (game.FONTSPEED * text.data[line].length()), 1);
    }
    else if (line < text.data.size() - 1) {
        ++line;
        progress = 0;
        text.reftick = game.ticks;
    }
    SDL_RenderFillRect(renderer, &textbox);
    for (int i = 0; i < line; ++i) {
        textrect.w = text.w[i];
        SDL_RenderTexture(renderer, text.texture[i], NULL, &textrect);
        textrect.y += text.h;
    }
    textsrcrect.w = text.w[line] * progress * 10;
    textrect.w = text.w[line] * progress;
    SDL_RenderTexture(renderer, text.texture[line], &textsrcrect, &textrect);
}

void Interface::clearText() {
    page = 0;
    line = 0;
    for (SDL_Texture *bits : text.texture) {
        SDL_DestroyTexture(bits);
        bits = NULL;
    }
}
