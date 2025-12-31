#include "Asset.h"

/* Loads a texture asset and adds it to a suitable container SDL_Texture list */
SDL_Texture* Asset::loadTexture(SDL_Renderer *renderer, std::string filePath) {
    char *path = NULL;
    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;

    SDL_asprintf(&path, filePath.c_str(), SDL_GetBasePath());

    surface = IMG_Load(path);
    if (!surface) {
        SDL_Log("Surface load failed: %s", SDL_GetError());
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Texture conversion failure: %s", SDL_GetError());
        return NULL;
    }

    SDL_free(path);
    SDL_DestroySurface(surface);
    return texture;
}

/* Load all textures from a pathList to a suitable container SDL_Texture list for quick reference. */
std::vector<SDL_Texture *> Asset::loadTexturesInto(SDL_Renderer *renderer, std::vector<std::string> pathList) {
    std::vector<SDL_Texture *> convertedList;
    for (int i = 0; i < pathList.size(); i++) {
        convertedList.push_back(loadTexture(renderer, pathList.at(i)));
    }
    return convertedList;
}
