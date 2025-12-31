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
    SDL_SetRenderLogicalPresentation(renderer, game.SCREENWIDTH, game.SCREENHEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    /* Load assets into SDL_Texture lists */
    Asset asset;
    playerSpriteList = asset.loadTexturesInto(renderer, game.SPRITEPATHS);
    npcSpriteList = asset.loadTexturesInto(renderer, game.NPCPATHS);
    mapTileList = asset.loadTexturesInto(renderer, game.TILEPATHS);

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

    /* Debug text */
    SDL_RenderDebugTextFormat(renderer, 10, 10, "x: %f", cam.x);
    SDL_RenderDebugTextFormat(renderer, 10, 20, "y: %f", cam.y);
    SDL_RenderDebugTextFormat(renderer, 10, 30, "tilex: %d", cam.tilex);
    SDL_RenderDebugTextFormat(renderer, 10, 40, "tiley: %d", cam.tiley);
    SDL_RenderDebugTextFormat(renderer, 10, 50, "facing: %d", cam.dir);
    SDL_RenderDebugTextFormat(renderer, 10, 60, "rendered tiles: %d", render.renderedTiles);
    SDL_SetRenderScale(renderer, 2, 2);
    SDL_RenderDebugText(renderer, 100, 50, "Waiting for something?");
    SDL_SetRenderScale(renderer, 1, 1);
    for (int i = 0; i < entityOrder.size(); i++) {
        SDL_RenderDebugTextFormat(renderer, 10, game.SCREENHEIGHT - (entityOrder.size() - i) * 10, "%s %.1f %.1f", entityOrder[i].name.c_str(), entityOrder[i].rect.x, entityOrder[i].rect.y);
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
