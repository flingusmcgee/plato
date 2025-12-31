#include "Camera.h"

/* Manage scrolling background/moving player based on keypresses and map borders */
void Camera::updateCamera(GameData& game, SDL_FRect& player, SDL_FRect& tile) {
    /* Controls */
    const bool *code = SDL_GetKeyboardState(NULL);
    if (code[SDL_SCANCODE_W] || code[SDL_SCANCODE_UP]) {
        y -= game.SPEED;
        dir = 0;
    }
    else if (code[SDL_SCANCODE_D] || code[SDL_SCANCODE_RIGHT]) {
        x += game.SPEED;
        dir = 90;
    }
    else if (code[SDL_SCANCODE_S] || code[SDL_SCANCODE_DOWN]) {
        y += game.SPEED;
        dir = 180;
    }
    else if (code[SDL_SCANCODE_A] || code[SDL_SCANCODE_LEFT]) {
        x -= game.SPEED;
        dir = 270;
    }

    /* Constrain the camera to map dimensions */
    tilex = 0;
    tiley = 0;
    playerx = 0;
    playery = 0;
    if (x >= 0 && x <= game.TILEWIDTH * game.MAPWIDTH - game.SCREENWIDTH) {
        tilex = -x;
        playerx = (game.SCREENWIDTH - game.SPRITEWIDTH) / 2;
    }
    else {
        if (x < 0) {
            tilex = 0;
            playerx = (game.SCREENWIDTH - game.SPRITEWIDTH) / 2 + x;
        }
        else {
            tilex = -(game.TILEWIDTH * game.MAPWIDTH - game.SCREENWIDTH);
            playerx = (game.SCREENWIDTH - game.SPRITEWIDTH) / 2 - (game.TILEWIDTH * game.MAPWIDTH - game.SCREENWIDTH) + x;
        }
    }
    if (y >= 0 && y <= game.TILEHEIGHT * game.MAPHEIGHT - game.SCREENHEIGHT) {
        tiley = -y;
        playery = (game.SCREENHEIGHT - game.SPRITEHEIGHT) / 2;
    }
    else {
        if (y < 0) {
            tiley = 0;
            playery = (game.SCREENHEIGHT - game.SPRITEHEIGHT) / 2 + y;
        }
        else {
            tiley = -(game.TILEHEIGHT * game.MAPHEIGHT - game.SCREENHEIGHT);
            playery = (game.SCREENHEIGHT - game.SPRITEHEIGHT) / 2 - (game.TILEHEIGHT * game.MAPHEIGHT - game.SCREENHEIGHT) + y;
        }
    }
    
    player.x = playerx;
    player.y = playery;
    tile.x = tilex;
    tile.y = tiley;
}
