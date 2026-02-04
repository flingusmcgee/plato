#include "Interface.h"

/* Creates text for an entity and toggles interaction boolean */
void Interface::createTextBox(SDL_Renderer *renderer, Game& game, int closest) {
    if (!game.interacting) {
        game.interacting = true;
    }
    else if (text.line < game.DIALOGUE[game.entityOrder[closest].type].size() - 1) {
        text.line++;
    }
    else {
        game.interacting = false;
        clearText();
    }
    if (game.interacting && closest != -1) {
        Asset asset;
        std::string textdata = game.DIALOGUE[game.entityOrder[closest].type][text.line];
        text = asset.loadFontTexture(renderer, font, textdata, { 0, 0, 0, 255 });
        text.reftick = game.ticks;
    }
}

/* Renders text background and animation */
void Interface::renderTextBox(SDL_Renderer *renderer, const Game& game, float height, float margin) {
    SDL_FRect textbox = { margin, game.SCREENHEIGHT - height, game.SCREENWIDTH - margin * 2, height - margin };
    SDL_FRect textsrcrect = { 0, 0, text.w * text.progress * 10, text.h * 10 };
    SDL_FRect textrect = { margin * 2, game.SCREENHEIGHT - height, text.w * text.progress, text.h };
    if (game.interacting) {
        SDL_RenderFillRect(renderer, &textbox);
        SDL_RenderTexture(renderer, text.texture, &textsrcrect, &textrect);
    }
}

void Interface::clearText() {
    text.line = 0;
    SDL_DestroyTexture(text.texture);
    text.texture = NULL;
}
