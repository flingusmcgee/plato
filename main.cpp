#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>
#include "src/Game.h"
#include "src/Reader.h"
#include "src/Asset.h"
#include "src/Camera.h"
#include "src/Render.h"
#include "src/Audio.h"
#include "src/Input.h"

static SDL_Window   *window    = NULL;
static SDL_Renderer *renderer  = NULL;
static Game game;
static Audio audio;
static Input input;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetAppMetadata("plato game", "0.0", "com.plato.elmegacorp");

    /* Initialize with external data */
    Reader reader;
    game.loadGameData(reader.readGameData());

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
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
    std::string textdata = "Waiting for something?";
    game.text = asset.loadFontTexture(renderer, font, textdata, { 0, 0, 0, 255 });
    TTF_CloseFont(font);

    audio.audioList.push_back(audio.loadAudio(game.AUDIOPATHS[0]));
    audio.createStream();

    input.initKeyboard();

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    if (event->type == SDL_EVENT_GAMEPAD_ADDED) {
        SDL_Log("Gamepad inserted: %u", event->gdevice.which);
        input.gamepad = SDL_OpenGamepad(event->gdevice.which);
        input.initGamepad();
        if (!input.gamepad) {
            SDL_Log("Gamepad not detected");
        }
    }
    if (event->type == SDL_EVENT_GAMEPAD_REMOVED) {
        SDL_Log("Gamepad disconnected");
        SDL_CloseGamepad(input.gamepad);
        input.gamepad = NULL;
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
    input.updateJoysticks();

    static Camera cam;
    cam.updateCamera(game, input, player, tile);

    /* Render the map tiles and process map-bound entities */
    static Render render;
    int renderedTiles = render.renderMap(renderer, game, game.entityOrder, tile, npc, cam.tilex);

    /* Ensure the player is the final processed entity to guarantee its index is known */
    int playeridx = render.orderEntity(game, game.entityOrder, game.SPRITEPATHS[cam.dir/90], game.playerSpriteList.at(cam.dir/90), player);
    
    /* Create the player interaction range */
    float range = 150;
    float width = 75;
    float horizontal = (cam.dir % 180 / 90) ? range : width;
    float vertical = (cam.dir % 180 / 90) ? width : range;

    int closest = game.getClosestTarget(game.entityOrder, playeridx, cam.dir, horizontal, vertical);
    /*
    SDL_FRect playerrange { normalize(player, 0) - horizontal, normalize(player, 1) - vertical, horizontal * 2, vertical * 2 };
    SDL_RenderFillRect(renderer, &playerrange);
    */

    /* Render the entities */
    for (Entity entity : game.entityOrder) {
        SDL_RenderTexture(renderer, entity.texture, NULL, &entity.rect);
    }

    /* In-game narration */
    float textmulti;

    if (input.isKeyDown(SDL_SCANCODE_Z, false) || input.isButtonDown(SDL_GAMEPAD_BUTTON_SOUTH, false)) {
        game.text.reftick = ticks;
        game.interacting = !game.interacting;
    }
    if (closest != -1) {
        SDL_FRect highlighter = game.entityOrder[closest].rect;
        highlighter.y += SDL_sinf(ticks / 300.0) * 10;
        SDL_RenderTexture(renderer, game.iconList[0], NULL, &highlighter);
    }
    else {
        game.interacting = false;
    }
    textmulti = SDL_min((ticks - game.text.reftick) / 500.0f, 1);
    SDL_FRect textbox  = { 10.0f, game.SCREENHEIGHT - 100.0f, game.SCREENWIDTH - 20.0f, 90.0f };
    SDL_FRect textsrcrect = { 0, 0, game.text.w * textmulti, game.text.h };
    SDL_FRect textrect = { 20.0f, game.SCREENHEIGHT - 100.0f, game.text.w * 0.1f * textmulti, game.text.h * 0.1f };
    if (game.interacting) {
        SDL_RenderFillRect(renderer, &textbox);
        SDL_RenderTexture(renderer, game.text.texture, &textsrcrect, &textrect);
    }

    /* Play audio */
    if (SDL_GetAudioStreamQueued(audio.stream) == 0) {
        audio.addAudio(audio.audioList[0]);
    }

    /* Line so it's canon */
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, 0, 0, game.SCREENWIDTH, game.SCREENHEIGHT);

    /* Debug text */
    SDL_RenderDebugTextFormat(renderer, 10, 10, "x: %.1f", cam.x);
    SDL_RenderDebugTextFormat(renderer, 10, 20, "y: %.1f", cam.y);
    SDL_RenderDebugTextFormat(renderer, 10, 30, "tilex: %.1f", cam.tilex);
    SDL_RenderDebugTextFormat(renderer, 10, 40, "tiley: %.1f", cam.tiley);
    SDL_RenderDebugTextFormat(renderer, 10, 50, "facing: %d", cam.dir);
    SDL_RenderDebugTextFormat(renderer, 10, 60, "rendered tiles / entities: %d / %d", renderedTiles, game.entityOrder.size());
    SDL_RenderDebugTextFormat(renderer, 10, 70, "input: %s", input.gamepad ? SDL_GetGamepadName(input.gamepad) : "keyboard");
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
    for (auto *bits : audio.audioList) {
        SDL_free(bits);
    }

    SDL_DestroyTexture(game.text.texture);
    TTF_Quit();

    SDL_CloseGamepad(input.gamepad);

    SDL_DestroyAudioStream(audio.stream);
    /* SDL will clean up the window/renderer for us. */
}
