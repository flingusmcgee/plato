#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>
#include "src/Game.h"
#include "src/Reader.h"
#include "src/Asset.h"
#include "src/Camera.h"
#include "src/Render.h"

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static Game game;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetAppMetadata("plato game", "0.0", "com.plato.elmegacorp");

    /* Initialize with external data */
    Reader reader;
    game.loadGameData(reader.readGameData());

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
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
        game.playerSpriteList = asset.loadTexturesInto(renderer, game.SPRITEPATHS);
    }
    if (!isEmpty(game.NPCPATHS, "Empty NPCPATHS")) {
        game.npcSpriteList = asset.loadTexturesInto(renderer, game.NPCPATHS);
    }
    if (!isEmpty(game.TILEPATHS, "Empty TILEPATHS")) {
        game.mapTileList = asset.loadTexturesInto(renderer, game.TILEPATHS);
    }
    if (!isEmpty(game.ICONPATHS, "Empty ICONPATHS")) {
        game.iconList = asset.loadTexturesInto(renderer, game.ICONPATHS);
    }

    TTF_Font *font = asset.loadFont("assets/Barriecito-Regular.ttf", 300); 
    game.text = asset.loadFontTexture(renderer, font, "Waiting for something?", { 0, 0, 0, 255 });

    TTF_CloseFont(font);
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    if (event->type == SDL_EVENT_GAMEPAD_ADDED) {
        SDL_Log("Gamepad inserted: %u", event->gdevice.which);
        gamepad = SDL_OpenGamepad(event->gdevice.which);
        if (!gamepad) {
            SDL_Log("Gamepad not detected");
        }
    }
    if (event->type == SDL_EVENT_GAMEPAD_REMOVED) {
        SDL_Log("Gamepad disconnected");
        SDL_CloseGamepad(gamepad);
        gamepad = NULL;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_SetRenderDrawColor(renderer, 249, 245, 236, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    const int ticks = SDL_GetTicks();

    SDL_FRect player { 0, 0, game.SPRITEWIDTH, game.SPRITEHEIGHT };
    SDL_FRect npc    { 0, 0, game.TILEWIDTH, game.TILEHEIGHT };
    SDL_FRect tile   { 0, 0, game.TILEWIDTH, game.TILEHEIGHT };

    /* Camera and controls */
    static Camera cam;
    cam.updateCamera(game, gamepad, player, tile);

    /* Render the map tiles and process map-bound entities */
    static Render render;
    int renderedTiles = render.renderMap(renderer, game, game.entityOrder, tile, npc, cam.tilex);

    /* Ensure the player is the final processed entity to guarantee its index is known */
    int playeridx = render.orderEntity(game, game.entityOrder, game.SPRITEPATHS[cam.dir/90], game.playerSpriteList.at(cam.dir/90), player);
    
    /* Create a cross shaped player interaction range */
    float range = 150; /* The length of each bar */
    float width = 50;  /* The width of each bar */
    float horizontal = (cam.dir % 180 / 90) ? range : width;
    float vertical = (cam.dir % 180 / 90) ? width : range;

    int closest = game.getClosestTarget(game, playeridx, cam.dir, horizontal, vertical);
    /*
    SDL_FRect playerrange { normalize(player, 0) - horizontal, normalize(player, 1) - vertical, horizontal * 2, vertical * 2 };
    SDL_RenderFillRect(renderer, &playerrange);
    */

    /* Render the entities */
    for (Entity entity : game.entityOrder) {
        SDL_RenderTexture(renderer, entity.texture, NULL, &entity.rect);
    }
    if (closest != -1) {
        SDL_FRect highlighter = game.entityOrder[closest].rect;
        highlighter.y += SDL_sinf(ticks / 300.0) * 10;
        SDL_RenderTexture(renderer, game.iconList[0], NULL, &highlighter);
    }

    /* Line so it's canon */
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, 0, 0, game.SCREENWIDTH, game.SCREENHEIGHT);

    /* In-game narration */
    SDL_FRect textbox = { 100, game.SCREENHEIGHT - 100.0f, game.text.w * 0.1f, game.text.h * 0.1f };
    SDL_RenderTexture(renderer, game.text.texture, NULL, &textbox);

    /* Debug text */
    SDL_RenderDebugTextFormat(renderer, 10, 10, "x: %.1f", cam.x);
    SDL_RenderDebugTextFormat(renderer, 10, 20, "y: %.1f", cam.y);
    SDL_RenderDebugTextFormat(renderer, 10, 30, "tilex: %.1f", cam.tilex);
    SDL_RenderDebugTextFormat(renderer, 10, 40, "tiley: %.1f", cam.tiley);
    SDL_RenderDebugTextFormat(renderer, 10, 50, "facing: %d", cam.dir);
    SDL_RenderDebugTextFormat(renderer, 10, 60, "rendered tiles / entities: %d / %d", renderedTiles, game.entityOrder.size());
    SDL_RenderDebugTextFormat(renderer, 10, 70, "input: %s", gamepad ? SDL_GetGamepadName(gamepad) : "keyboard");
    SDL_RenderDebugTextFormat(renderer, 10, 80, "dist: %.2f", game.getDistance(player, game.entityOrder[0].rect));
    SDL_RenderDebugTextFormat(renderer, 10, 90, "playeridx: %d", playeridx);
    for (int i = 0; i < game.above.size(); i++) {
        SDL_RenderDebugTextFormat(renderer, 10 + 10 * i, 100, "%d", game.above[i]);
    }
    for (int i = 0; i < game.below.size(); i++) {
        SDL_RenderDebugTextFormat(renderer, 10 + 10 * i, 110, "%d", game.below[i]);
    }
    for (int i = 0; i < game.entityOrder.size(); ++i) {
        SDL_RenderDebugTextFormat(renderer, 10, game.SCREENHEIGHT - (game.entityOrder.size() - i) * 10, "%.*s %.1f %.1f", static_cast<int>(game.entityOrder[i].name.length()), game.entityOrder[i].name.data(), game.entityOrder[i].rect.x, game.entityOrder[i].rect.y);
    }

    /* Show time! */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    for (SDL_Texture *bits : game.playerSpriteList) {
        SDL_DestroyTexture(bits);
    }
    for (SDL_Texture *bits : game.npcSpriteList) {
        SDL_DestroyTexture(bits);
    }
    for (SDL_Texture *bits : game.mapTileList) {
        SDL_DestroyTexture(bits);
    }
    for (SDL_Texture *bits : game.iconList) {
        SDL_DestroyTexture(bits);
    }

    SDL_DestroyTexture(game.text.texture);
    TTF_Quit();

    SDL_CloseGamepad(gamepad);
    /* SDL will clean up the window/renderer for us. */
}
