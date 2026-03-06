#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>
#include "src/Asset.h"
#include "src/Audio.h"
#include "src/Camera.h"
#include "src/EntityManager.h"
#include "src/Game.h"
#include "src/Input.h"
#include "src/Interface.h"
#include "src/Reader.h"
#include "src/Render.h"

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static Game game;
static EntityManager entityManager;
static Audio audio;
static Input input;
static Interface interface;

static std::vector<Entity> entityOrder;

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
    if (!isEmpty(game.AUDIOPATHS, "Empty AUDIOPATHS")) {
        audio.createStream();
    }

    interface.font = asset.loadFont(game.FONTPATH, game.FONTSIZE);

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
        input.initGamepad(SDL_OpenGamepad(event->gdevice.which));
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

    game.ticks = SDL_GetTicks();
    static int lastframe;
    int fps = game.ticks - lastframe;
    lastframe = game.ticks;

    SDL_FRect player { 0, 0, (float) game.SPRITEWIDTH, (float) game.SPRITEHEIGHT };
    SDL_FRect npc    { 0, 0, (float) game.TILEWIDTH,   (float) game.TILEHEIGHT };
    SDL_FRect tile   { 0, 0, (float) game.TILEWIDTH,   (float) game.TILEHEIGHT };
    static SDL_FRect enemy  { 50, 50, (float) game.TILEWIDTH, (float) game.TILEWIDTH };

    /* Camera and controls */
    input.updateJoysticks();

    static Camera cam;
    Coordinate origin = cam.updateCamera(game, input, player);
    tile = offset(tile, origin);

    /* IMPORTANT: DO NOT PROCESS ENTITIES BEFORE THIS POINT */
    /* Render the map tiles and process map-bound entities */
    static Render render;
    int renderedTiles = render.renderMap(renderer, game, entityOrder, tile, npc, origin.x);

    /* Process a temporary enemy character */
    float speedScale = SDL_min(entityManager.getDistance(offset(enemy, origin), player) / game.RANGE, 1); /* Move further objects faster */
    float angle = entityManager.getDirection(enemy, player, origin);
    if (entityManager.getDistance(offset(enemy, origin), player) > game.RANGE / 2) {
        enemy = entityManager.move(enemy, angle, game.ENEMYSPEED * speedScale);
    }
    render.orderEntity(entityOrder, 2, game.npcSpriteList.at(2), offset(enemy, origin));

    /* Ensure the player is the final processed entity to guarantee its index is known */
    int playeridx = render.orderEntity(entityOrder, 0, game.playerSpriteList.at(cam.dir/90), player);
    
    /* Create the player interaction range */
    float horizontal = (cam.dir % 180) ? game.RANGE : game.BREADTH;
    float vertical = (cam.dir % 180) ? game.BREADTH : game.RANGE;

    int closest = entityManager.getClosestTarget(entityOrder, playeridx, cam.dir, horizontal, vertical);
    /*
    SDL_FRect playerrange { normalize(player, 0) - horizontal, normalize(player, 1) - vertical, horizontal * 2, vertical * 2 };
    SDL_RenderFillRect(renderer, &playerrange);
    */

    /* Render the entities */
    for (Entity entity : entityOrder) {
        SDL_RenderTexture(renderer, entity.texture, NULL, &entity.rect);
    }

    /* Interactivity and narration */
    const float TEXTBOXHEIGHT = 120.0f;
    const float TEXTBOXMARGIN = 10.0f;
    if (closest != -1 && !game.DIALOGUE[entityOrder[closest].type].empty()) {
        SDL_FRect highlighter = entityOrder[closest].rect;
        highlighter.y += SDL_sinf((float) game.ticks / 300.0f) * 10;
        SDL_RenderTexture(renderer, game.iconList[0], NULL, &highlighter);
        if (input.isKeyDown(SDL_SCANCODE_Z, false) || input.isButtonDown(SDL_GAMEPAD_BUTTON_SOUTH, false)) {
            interface.createTextBox(renderer, game, entityOrder, closest, game.SCREENWIDTH - TEXTBOXMARGIN * 3);
        }
        if (game.interacting) {
            interface.renderTextBox(renderer, game, TEXTBOXHEIGHT, TEXTBOXMARGIN);
        }
    }

    /* Play audio */
    audio.bgtype = 1;
    if (SDL_GetAudioStreamQueued(audio.stream) == 0) {
        SDL_free(audio.bg);
        audio.bg = audio.loadAudio(game.AUDIOPATHS[audio.bgtype]);
        audio.addAudio(audio.bg);
    }

    /* Line so it's canon */
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, 0, 0, game.SCREENWIDTH, game.SCREENHEIGHT);
    SDL_RenderPoint(renderer, normalize(offset(enemy, origin), 0), normalize(offset(enemy, origin), 1));
    SDL_RenderPoint(renderer, normalize(player, 0), normalize(player, 1));

    /* Debug text */
    SDL_RenderDebugTextFormat(renderer, 10, 10, "fps: %d", (fps) ? 1000 / fps : 0);
    SDL_RenderDebugTextFormat(renderer, 10, 20, "x: %.1f", cam.x);
    SDL_RenderDebugTextFormat(renderer, 10, 30, "y: %.1f", cam.y);
    SDL_RenderDebugTextFormat(renderer, 10, 40, "facing: %d", cam.dir);
    SDL_RenderDebugTextFormat(renderer, 10, 50, "rendered tiles / entities: %d / %d", renderedTiles, entityOrder.size());
    SDL_RenderDebugTextFormat(renderer, 10, 60, "input: %s", input.gamepad ? SDL_GetGamepadName(input.gamepad) : "keyboard");
    for (int i = 0; i < entityOrder.size(); ++i) {
        SDL_RenderDebugTextFormat(renderer, 10, game.SCREENHEIGHT - (entityOrder.size() - i) * 10, "%s %.1f %.1f", game.NPCSET.at(entityOrder[i].type).c_str(), entityOrder[i].rect.x, entityOrder[i].rect.y);
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
    for (SDL_Texture *bits : interface.text.texture) {
        SDL_DestroyTexture(bits);
    }
    TTF_CloseFont(interface.font);
    TTF_Quit();

    SDL_CloseGamepad(input.gamepad);

    SDL_free(audio.bg);
    SDL_DestroyAudioStream(audio.stream);
    /* SDL will clean up the window/renderer for us. */
}
