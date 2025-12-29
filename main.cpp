#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include "main.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture  *texture  = NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    /* Initialize with external data */
    Reader reader;
    loadGameData(reader.readGameData());

    SDL_SetAppMetadata("plato game", "0.0", "com.plato.elmegacorp");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Plato", SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, SCREENWIDTH, SCREENHEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    /* Load assets into SDL_Texture lists */
    for (int i = 0; i < SPRITEPATHS.size(); i++) {
        loadTexture(SPRITEPATHS.at(i), &playerSpriteList);
    }
    for (int i = 0; i < NPCPATHS.size(); i++) {
        loadTexture(NPCPATHS.at(i), &npcSpriteList);
    }
    for (int i = 0; i < TILEPATHS.size(); i++) {
        loadTexture(TILEPATHS.at(i), &mapTileList);
    }

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
    static float camx { 0 };
    static float camy { 0 };
    static int dir { 0 };

    /* Controls: Fix me! */
    const bool *code = SDL_GetKeyboardState(NULL);
    if (code[SDL_SCANCODE_W]) {
        camy -= SPEED;
        dir = 0;
    }
    else if (code[SDL_SCANCODE_D]) {
        camx += SPEED;
        dir = 90;
    }
    else if (code[SDL_SCANCODE_S]) {
        camy += SPEED;
        dir = 180;
    }
    else if (code[SDL_SCANCODE_A]) {
        camx -= SPEED;
        dir = 270;
    }

    SDL_SetRenderDrawColor(renderer, 249, 245, 236, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_FRect player;
    SDL_FRect npc;
    SDL_FRect tile;
    entityOrder = { };

    player.w = SPRITEWIDTH;
    player.h = SPRITEHEIGHT;
    npc.w = TILEWIDTH;
    npc.h = TILEHEIGHT;
    tile.w = TILEWIDTH;
    tile.h = TILEHEIGHT;

    /* Constrain the camera to map dimensions */
    int tilex, tiley;
    int playerx, playery;
    if (camx >= 0 && camx <= TILEWIDTH * MAPWIDTH - SCREENWIDTH) {
        tilex = -camx;
        playerx = (SCREENWIDTH - SPRITEWIDTH) / 2;
    }
    else {
        if (camx < 0) {
            tilex = 0;
            playerx = (SCREENWIDTH - SPRITEWIDTH) / 2 + camx;
        }
        else {
            tilex = -(TILEWIDTH * MAPWIDTH - SCREENWIDTH);
            playerx = (SCREENWIDTH - SPRITEWIDTH) / 2 - (TILEWIDTH * MAPWIDTH - SCREENWIDTH) + camx;
        }
    }
    if (camy >= 0 && camy <= TILEHEIGHT * MAPHEIGHT - SCREENHEIGHT) {
        tiley = -camy;
        playery = (SCREENHEIGHT - SPRITEHEIGHT) / 2;
    }
    else {
        if (camy < 0) {
            tiley = 0;
            playery = (SCREENHEIGHT - SPRITEHEIGHT) / 2 + camy;
        }
        else {
            tiley = -(TILEHEIGHT * MAPHEIGHT - SCREENHEIGHT);
            playery = (SCREENHEIGHT - SPRITEHEIGHT) / 2 - (TILEHEIGHT * MAPHEIGHT - SCREENHEIGHT) + camy;
        }
    }
    player.x = playerx;
    player.y = playery;
    tile.x = tilex;
    tile.y = tiley;

    /* Render the map tiles */
    int renderedTiles = 0;
    for (int i = 0; i < MAPHEIGHT; i++) {
        for (int j = 0; j < MAPWIDTH; j++) {
            if (tile.x > -TILEWIDTH && tile.x < SCREENWIDTH && tile.y > -TILEHEIGHT && tile.y < SCREENHEIGHT) {
                SDL_RenderTexture(renderer, mapTileList.at(MAP[i][j]), NULL, &tile);
                /* Queue entity processing if applicable */
                npc.x = tile.x;
                npc.y = tile.y;
                if (NPC[i][j] > EMPTY) {
                    orderEntity(NPCPATHS[NPC[i][j]], &npcSpriteList.at(NPC[i][j]), npc);
                }
                renderedTiles += 1;
            }
            tile.x += TILEWIDTH;
        }
        tile.x = tilex;
        tile.y += TILEHEIGHT;
    }

    /* Render the entities */
    orderEntity(SPRITEPATHS[dir/90], &playerSpriteList.at(dir/90), player);
    for (Entity entity : entityOrder) {
        SDL_RenderTexture(renderer, *entity.texture, NULL, &entity.rect);
    }

    /* Line so it's canon */
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, 0, 0, SCREENWIDTH, SCREENHEIGHT);

    /* Debug text */
    SDL_RenderDebugTextFormat(renderer, 10, 10, "x: %f", camx);
    SDL_RenderDebugTextFormat(renderer, 10, 20, "y: %f", camy);
    SDL_RenderDebugTextFormat(renderer, 10, 30, "tilex: %d", tilex);
    SDL_RenderDebugTextFormat(renderer, 10, 40, "tiley: %d", tiley);
    SDL_RenderDebugTextFormat(renderer, 10, 50, "facing: %d", dir);
    SDL_RenderDebugTextFormat(renderer, 10, 60, "rendered tiles: %d", renderedTiles);
    SDL_SetRenderScale(renderer, 2, 2);
    SDL_RenderDebugText(renderer, 100, 50, "Waiting for something?");
    SDL_SetRenderScale(renderer, 1, 1);
    for (int i = 0; i < entityOrder.size(); i++) {
        SDL_RenderDebugTextFormat(renderer, 10, SCREENHEIGHT - (entityOrder.size() - i) * 10, "%s %.1f %.1f", entityOrder[i].name.c_str(), entityOrder[i].rect.x, entityOrder[i].rect.y);
    }

    /* Show time! */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    SDL_DestroyTexture(texture);
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
static void loadGameData(ExternalData external) {
    SCREENWIDTH  = external.SCREENWIDTH;
    SCREENHEIGHT = external.SCREENHEIGHT;
    SPEED        = external.SPEED;
    SPRITEPATHS  = external.SPRITEPATHS;
    NPCPATHS     = external.NPCPATHS;
    TILEPATHS    = external.TILEPATHS;
    SPRITEWIDTH  = external.SPRITEWIDTH;
    SPRITEHEIGHT = external.SPRITEHEIGHT;
    TILEWIDTH    = external.TILEWIDTH;
    TILEHEIGHT   = external.TILEHEIGHT;
    MAPWIDTH     = external.MAPWIDTH;
    MAPHEIGHT    = external.MAPHEIGHT;
    MAP          = external.MAP;
    NPC          = external.NPC;
}

/* Loads texture assets and adds them to a suitable container SDL_Texture list for quick reference. */
static SDL_AppResult loadTexture(std::string filePath, std::vector<SDL_Texture *> *destination) {
    char *path = NULL;
    SDL_Surface *surface = NULL;

    SDL_asprintf(&path, filePath.c_str(), SDL_GetBasePath());

    surface = IMG_Load(path);
    if (!surface) {
        SDL_Log("Surface load failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Texture conversion failure: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    destination->push_back(texture);

    SDL_free(path);
    SDL_DestroySurface(surface);
    return SDL_APP_CONTINUE;
}

/* Aligns entities to a pseudo z-index in preparation of rendering */
static void orderEntity(std::string name, SDL_Texture **texture, SDL_FRect rect) {
    if (!entityOrder.size()) {
        entityOrder.push_back({name, texture, rect});
    }
    else {
        auto i = entityOrder.begin();
        for (Entity entity : entityOrder) {
            if (rect.y - (TILEHEIGHT - rect.h) <= entity.rect.y) {
                entityOrder.insert(i, {name, texture, rect});
                break;
            }
            i++;
        }
        if (rect.y - (TILEHEIGHT - rect.h) > entityOrder.back().rect.y) {
            entityOrder.push_back({name, texture, rect});
        }
    }
}
