#include "Asset.h"

/* Loads a texture asset and adds it to a suitable container SDL_Texture list */
SDL_Texture* Asset::loadTexture(SDL_Renderer *renderer, std::string filePath) {
    if (filePath.empty()) {
        return NULL;
    }
    
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
        SDL_Log("Texture conversion failed: %s", SDL_GetError());
        return NULL;
    }

    SDL_free(path);
    SDL_DestroySurface(surface);
    return texture;
}

/* Load all textures from a pathList to a suitable container SDL_Texture list for quick reference. */
std::vector<SDL_Texture *> Asset::loadTexturesInto(SDL_Renderer *renderer, std::vector<std::string> pathList) {
    std::vector<SDL_Texture *> convertedList;
    for (std::string path : pathList) {
        convertedList.push_back(loadTexture(renderer, path));
    }
    return convertedList;
}

TTF_Font* Asset::loadFont(std::string fontPath, float fontSize) {
    return TTF_OpenFont(fontPath.c_str(), fontSize * 10);
}

/* Load a Text object with line-separated dialogue and attributes */
Text Asset::loadTextTexture(SDL_Renderer *renderer, TTF_Font* font, std::string text, SDL_Color color, int maxWidth) {
    /* Separate dialogue for line wrapping */
    std::vector<std::string> data;
    std::string remaining = text;
    std::string chunk;
    size_t chunkLen;
    int totalLen = 0;
    while (totalLen < text.size() - 1) {
        TTF_MeasureString(font, remaining.c_str(), 0, maxWidth * 10, NULL, &chunkLen);
        totalLen += chunkLen;
        chunk = remaining.substr(0, chunkLen);
        if (totalLen < text.size() - 1) {
            for (int i = 1; i < chunk.size(); ++i) {
                if (chunk[chunk.size() - i] == ' ') {
                    chunk = chunk.substr(0, chunk.size() - i);
                    totalLen -= i - 1;
                    break;
                }
            }
        }
        data.push_back(chunk);
        remaining = text.substr(totalLen);
    }

    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;
    std::vector<SDL_Texture *> textures;
    std::vector<float> w;
    float h;
    for (std::string string : data) {
        surface = TTF_RenderText_Blended(font, string.c_str(), 0, color);
        if (!surface) {
            SDL_Log("Surface load failed: %s", SDL_GetError());
            return { };
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            SDL_Log("Texture conversion failed: %s", SDL_GetError());
            SDL_DestroySurface(surface);
            return { };
        }
        textures.push_back(texture);
        w.push_back((float) surface->w / 10);
        h = (float) surface->h / 10;
        SDL_DestroySurface(surface);
    }

    return { textures, data, w, h };
}
