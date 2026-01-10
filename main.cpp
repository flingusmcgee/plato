#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>
#include "main.h"
#include "src/Reader.h"
#include "src/Asset.h"
#include "src/Camera.h"
#include "src/Render.h"

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static GameData game;
static Text text;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetAppMetadata("plato game", "0.0", "com.plato.elmegacorp");

    /* Initialize with external data */
    Reader reader;
    loadGameData(reader.readGameData());

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Plato", game.SCREENWIDTH, game.SCREENHEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL_ttf: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, game.SCREENWIDTH, game.SCREENHEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    /* Load assets into SDL_Texture lists */
    Asset asset;
    if (!isEmpty(game.SPRITEPATHS, "Empty SPRITEPATHS")) {
        playerSpriteList = asset.loadTexturesInto(renderer, game.SPRITEPATHS);
    }
    if (!isEmpty(game.NPCPATHS, "Empty NPCPATHS")) {
        npcSpriteList = asset.loadTexturesInto(renderer, game.NPCPATHS);
    }
    if (!isEmpty(game.TILEPATHS, "Empty TILEPATHS")) {
        mapTileList = asset.loadTexturesInto(renderer, game.TILEPATHS);
    }

    TTF_Font *font = asset.loadFont("assets/Barriecito-Regular.ttf", 300); 
    text = asset.loadFontTexture(renderer, font, "Waiting for something?", { 0, 0, 0, 255 });

    TTF_CloseFont(font);
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_SetRenderDrawColor(renderer, 249, 245, 236, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_FRect player;
    SDL_FRect npc;
    SDL_FRect tile;

    player.w = game.SPRITEWIDTH;
    player.h = game.SPRITEHEIGHT;
    npc.w = game.TILEWIDTH;
    npc.h = game.TILEHEIGHT;
    tile.w = game.TILEWIDTH;
    tile.h = game.TILEHEIGHT;

    /* Camera and controls */
    static Camera cam;
    cam.updateCamera(game, player, tile);

    /* Render the map tiles */
    static Render render;
    entityOrder = render.renderMap(game, renderer, tile, mapTileList, npc, npcSpriteList, cam.tilex);

    /* Render the entities */
    entityOrder = render.orderEntity(game, game.SPRITEPATHS[cam.dir/90], playerSpriteList.at(cam.dir/90), player);
    for (Entity entity : entityOrder) {
        SDL_RenderTexture(renderer, entity.texture, NULL, &entity.rect);
    }

    /* Line so it's canon */
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, 0, 0, game.SCREENWIDTH, game.SCREENHEIGHT);

    /* In-game narration */
    SDL_FRect textbox = { 100, game.SCREENHEIGHT - 100.0f, text.w * 0.1f, text.h * 0.1f };
    SDL_RenderTexture(renderer, text.texture, NULL, &textbox);

    /* Debug text */
    SDL_RenderDebugTextFormat(renderer, 10, 10, "x: %f", cam.x);
    SDL_RenderDebugTextFormat(renderer, 10, 20, "y: %f", cam.y);
    SDL_RenderDebugTextFormat(renderer, 10, 30, "tilex: %d", cam.tilex);
    SDL_RenderDebugTextFormat(renderer, 10, 40, "tiley: %d", cam.tiley);
    SDL_RenderDebugTextFormat(renderer, 10, 50, "facing: %d", cam.dir);
    SDL_RenderDebugTextFormat(renderer, 10, 60, "rendered tiles: %d", render.renderedTiles);
    for (int i = 0; i < entityOrder.size(); ++i) {
        SDL_RenderDebugTextFormat(renderer, 10, game.SCREENHEIGHT - (entityOrder.size() - i) * 10, "%.*s %.1f %.1f", static_cast<int>(entityOrder[i].name.length()), entityOrder[i].name.data(), entityOrder[i].rect.x, entityOrder[i].rect.y);
    }

    /* Show time! */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    for (SDL_Texture *bits : playerSpriteList) {
        SDL_DestroyTexture(bits);
    }
    for (SDL_Texture *bits : npcSpriteList) {
        SDL_DestroyTexture(bits);
    }
    for (SDL_Texture *bits : mapTileList) {
        SDL_DestroyTexture(bits);
    }

    SDL_DestroyTexture(text.texture);
    TTF_Quit();
    /* SDL will clean up the window/renderer for us. */
}

/* Initialize game data using reader.cpp */
static void loadGameData(GameData external) {
    game.SCREENWIDTH  = external.SCREENWIDTH;
    game.SCREENHEIGHT = external.SCREENHEIGHT;
    game.SPEED        = external.SPEED;
    game.SPRITEPATHS  = external.SPRITEPATHS;
    game.NPCPATHS     = external.NPCPATHS;
    game.TILEPATHS    = external.TILEPATHS;
    game.SPRITEWIDTH  = external.SPRITEWIDTH;
    game.SPRITEHEIGHT = external.SPRITEHEIGHT;
    game.TILEWIDTH    = external.TILEWIDTH;
    game.TILEHEIGHT   = external.TILEHEIGHT;
    game.MAPWIDTH     = external.MAPWIDTH;
    game.MAPHEIGHT    = external.MAPHEIGHT;
    game.MAP          = external.MAP;
    game.NPC          = external.NPC;
}

/**
 * Flexible emptiness check for safe handling - Multi-Dimensional Edition
 * \param container multi-dimensional data structure to be tested
 * \param messageTotal log message on total emptiness
 * \param messageComponent log message on individual emptiness
 */
template <typename T>
static bool isEmpty(std::vector<std::vector<T>> container, const char *messageTotal, const char *messageComponent) {
    bool value = isEmpty(container, messageTotal);
    if (!value) {
        int i = 0;
        for (auto row : container) {
            if (row.empty()) {
                SDL_Log("%s %d", messageComponent, i);
                value = true;
                i++;
            }
        }
    }
    return value;
}
/**
 * Flexible emptiness check for safe handling
 * \param container data structure to be tested
 * \param message log message on total emptiness
 */
template <typename T>
static bool isEmpty(T container, const char *message) {
    if (container.empty()) {
        SDL_Log("%s", message);
        return true;
    }
    return false;
}
