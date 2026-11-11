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
#include "src/Utility.h"

static SDL_Window   *window   = NULL;
static SDL_Renderer *renderer = NULL;
static Game game;
static Audio audio;
static Input input;
static Interface interface;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetAppMetadata("plato game", "0.0", "com.plato.elmegacorp");

    /* Initialize with external data */
    Reader reader;
    if (!reader.readGameData(game)) {
        SDL_Log("Couldn't load game data: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "OPENGL, VULKAN");

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

    game.ticks = SDL_GetTicksNS();
    static Uint64 lastframe;
    game.delta = (float) (game.ticks - lastframe) / NSCOEFFICIENT;
    lastframe = game.ticks;

    game.entityOrder = { };
    input.updateJoysticks();

    /* Define the player and the camera */
    const Vector playerSpawn = { game.SCREENWIDTH / 2, game.SCREENHEIGHT / 2 };
    static Entity player(game.ENTITYTEMPLATES["TIMMY"], playerSpawn);
    static Camera cam;
    cam.updateCamera(game, player.prect);

    /* Render the map tiles and initialize map-bound entities */
    static Render render;
    int renderedTiles = render.renderMap(renderer, game, cam);

    /* Process all entity characters */
    for (auto& [id, entity] : game.existingEntities) {
        if (entity.group == "enemy") {
            entity.applyDisplacement(entity.getChaseVector(player) * game.delta, game.entityOrder);
            entity.updateEntity(cam);
        }
        entity.setRenderPosition(cam);
        if (game.isInView(entity.drect)) {
            render.orderEntity(game.entityOrder, &entity);
        }
    }

    /* Ensure the player is the final processed entity to guarantee its index is known */
    player.applyDisplacement(input.getMoveControls(game) * player.speed * game.delta, game.entityOrder);
    player.setSprite();
    player.updateEntity(cam);
    int playeridx = render.orderEntity(game.entityOrder, &player);
    int closestidx = player.getClosestTarget(game.entityOrder, "npc", playeridx, player.dir, player.range);

    /* Render the entities */
    for (Entity* entity : game.entityOrder) {
        if (game.isInView(entity->drect)) {
            SDL_RenderTexture(renderer, entity->textures.at(entity->texture), NULL, &entity->drect);
        }
    }

    /* Interactivity and narration */
    const float TEXTBOXHEIGHT = 120.0f;
    const float TEXTBOXMARGIN = 10.0f;
    if (closestidx != -1) {
        Entity* closest = game.entityOrder[closestidx];
        if (!game.DIALOGUE[closest->type].empty()) {
            game.highlight(renderer, closest->drect);
            if (input.isKeyDown(SDL_SCANCODE_Z, false) || input.isButtonDown(SDL_GAMEPAD_BUTTON_SOUTH, false)) {
                interface.createTextBox(renderer, game, closest, game.SCREENWIDTH - TEXTBOXMARGIN * 3);
            }
            if (game.interacting) {
                interface.renderTextBox(renderer, game, TEXTBOXHEIGHT, TEXTBOXMARGIN);
            }
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

    /* Show boxes */
    if (game.SHOWBOXES) {
        render.renderCircle(renderer, cam.offsetToCamera(player.prect).x, cam.offsetToCamera(player.prect).y, player.range);
        for (Entity* entity : game.entityOrder) {
            render.renderBigPoint(renderer, cam.offsetToCamera(entity->prect).x, cam.offsetToCamera(entity->prect).y, 6);
            SDL_FRect hitbox { cam.offsetToCamera(entity->hitbox).x, cam.offsetToCamera(entity->hitbox).y, entity->hitbox.w, entity->hitbox.h };
            if (entity->hitboxLayer == 1) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
            }
            if (entity->hitboxLayer == 2) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
            }
            if (entity->hitboxLayer == 3) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
            }
            SDL_RenderRect(renderer, &hitbox);
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
        }
    }

    /* Debug text */
    SDL_RenderDebugTextFormat(renderer, 10, 10, "fps: %.1f", (game.delta) ? 1.0f / game.delta : 0);
    SDL_RenderDebugTextFormat(renderer, 10, 20, "x: %.1f", player.prect.x);
    SDL_RenderDebugTextFormat(renderer, 10, 30, "y: %.1f", player.prect.y);
    SDL_RenderDebugTextFormat(renderer, 10, 40, "facing: %.1f", player.dir * 180 / SDL_PI_F);
    SDL_RenderDebugTextFormat(renderer, 10, 50, "cam x: %.1f", cam.origin.x);
    SDL_RenderDebugTextFormat(renderer, 10, 60, "cam y: %.1f", cam.origin.y);
    SDL_RenderDebugTextFormat(renderer, 10, 70, "rendered tiles / entities: %d / %zu", renderedTiles, game.entityOrder.size());
    SDL_RenderDebugTextFormat(renderer, 10, 80, "total entities: %zu", game.existingEntities.size() + 1);
    SDL_RenderDebugTextFormat(renderer, 10, 90, "input: %s", input.gamepad ? SDL_GetGamepadName(input.gamepad) : "keyboard");

    for (int i = 0; i < game.entityOrder.size(); ++i) {
        SDL_RenderDebugTextFormat(renderer, 10, game.SCREENHEIGHT - (game.entityOrder.size() - i) * 10, "%s %.1f %.1f", game.NPCSET.at(game.entityOrder[i]->type).c_str(), game.entityOrder[i]->prect.x, game.entityOrder[i]->prect.y);
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
