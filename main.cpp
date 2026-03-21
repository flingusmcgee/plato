#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>
#include "src/Asset.h"
#include "src/Audio.h"
#include "src/Camera.h"
#include "src/Entity.h"
#include "src/Game.h"
#include "src/Input.h"
#include "src/Interface.h"
#include "src/Reader.h"
#include "src/Render.h"

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static Game game;
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
    if (!isEmpty(game.ENTITYTEMPLATES, "Empty ENTITYTEMPLATES")) {
        for (auto& [_, entityTemplate] : game.ENTITYTEMPLATES) {
            if (entityTemplate.type > 0) {
                entityTemplate.textures = asset.loadTexturesInto(renderer, entityTemplate.texturePaths);
            }
        }
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

    /* Camera and controls */
    static Camera cam;
    Entity player(game.ENTITYTEMPLATES.at("TIMMY"));
    input.updateJoysticks();
    cam.updateCamera(game, input, player.prect, player.speed);
    player.updateEntity({ }, cam.dir / 90);

    if (input.isKeyDown(SDL_SCANCODE_E, false)) {
        cam.reftick = game.ticks;
    }
    if (input.isKeyDown(SDL_SCANCODE_E, true)) {
        cam.shakeCamera(10, game.ticks);
    }
    /* x * cos(angle) - y * sin(angle), x * sin(angle) + y * cos(angle) */

    /* Render the map tiles and process map-bound entities */
    static Render render;
    int renderedTiles = render.renderMap(renderer, game, entityOrder, translate(cam.origin, cam.shakeOffset));

    /* Process a temporary enemy character */
    static Entity enemy(game.ENTITYTEMPLATES.at("TEETH"));
    float speedScale = SDL_min(enemy.getDistance(player.prect) / player.range, 1); /* Move further objects faster */
    float angle = enemy.getDirection(player.prect);
    if (speedScale < 0.5) {
        speedScale = 0;
    }
    enemy.updateEntity(getMoveOffset(angle, enemy.speed * speedScale), 0);
    render.orderEntity(entityOrder, enemy);

    /* Ensure the player is the final processed entity to guarantee its index is known */
    int playeridx = render.orderEntity(entityOrder, player);

    /* Render the entities */
    for (Entity& entity : entityOrder) {
        entity.drect = cam.offset(getRenderAnchor(entity.prect));
        if (isInView(entity.drect, game)) {
            SDL_RenderTexture(renderer, entity.textures.at(entity.texture), NULL, &entity.drect);
        }
    }

    /* Create the player interaction range */
    const float HORIZONTAL = (cam.dir % 180) ? player.range : player.breadth;
    const float VERTICAL = (cam.dir % 180) ? player.breadth : player.range;
    
    // SDL_FRect playerrange { cam.offset(player.prect).x - horizontal, cam.offset(player.prect).y - vertical, horizontal * 2, vertical * 2 };
    // SDL_RenderFillRect(renderer, &playerrange);

    int closest = player.getClosestTarget(entityOrder, "npc", playeridx, cam.dir, HORIZONTAL, VERTICAL);

    /* Interactivity and narration */
    const float TEXTBOXHEIGHT = 120.0f;
    const float TEXTBOXMARGIN = 10.0f;
    if (closest != -1 && !game.DIALOGUE[entityOrder[closest].type].empty()) {
        entityOrder[closest].highlight(renderer, game);
        if (input.isKeyDown(SDL_SCANCODE_Z, false) || input.isButtonDown(SDL_GAMEPAD_BUTTON_SOUTH, false)) {
            interface.createTextBox(renderer, game, entityOrder[closest], game.SCREENWIDTH - TEXTBOXMARGIN * 3);
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

    /* Debug text */
    SDL_RenderDebugTextFormat(renderer, 10, 10, "fps: %d", (fps) ? 1000 / fps : 0);
    SDL_RenderDebugTextFormat(renderer, 10, 20, "x: %.1f", player.prect.x);
    SDL_RenderDebugTextFormat(renderer, 10, 30, "y: %.1f", player.prect.y);
    SDL_RenderDebugTextFormat(renderer, 10, 40, "facing: %d", cam.dir);
    SDL_RenderDebugTextFormat(renderer, 10, 50, "cam x: %.1f", cam.origin.x);
    SDL_RenderDebugTextFormat(renderer, 10, 60, "cam y: %.1f", cam.origin.y);
    SDL_RenderDebugTextFormat(renderer, 10, 70, "rendered tiles / entities: %d / %d", renderedTiles, entityOrder.size());
    SDL_RenderDebugTextFormat(renderer, 10, 80, "input: %s", input.gamepad ? SDL_GetGamepadName(input.gamepad) : "keyboard");
    SDL_RenderDebugTextFormat(renderer, 10, 90, "collision: %d", player.isColliding(enemy.hitbox) ? 1 : 0);
    for (int i = 0; i < entityOrder.size(); ++i) {
        SDL_RenderDebugTextFormat(renderer, 10, game.SCREENHEIGHT - (entityOrder.size() - i) * 10, "%s %.1f %.1f", game.NPCSET.at(entityOrder[i].type).c_str(), entityOrder[i].prect.x, entityOrder[i].prect.y);
    }

    /* Show time! */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    for (auto& [_, entityTemplate] : game.ENTITYTEMPLATES) {
        for (SDL_Texture *bits : entityTemplate.textures) {
            SDL_DestroyTexture(bits);
        }
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
