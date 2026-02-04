#include "Camera.h"

/* Manage scrolling background/moving player based on keypresses and map borders */
void Camera::updateCamera(Game& game, Input& input, SDL_FRect& player, SDL_FRect& tile) {
    /* Controls */
    bool up    = input.isKeyDown(SDL_SCANCODE_W, true) || input.isKeyDown(SDL_SCANCODE_UP, true);
    bool right = input.isKeyDown(SDL_SCANCODE_D, true) || input.isKeyDown(SDL_SCANCODE_RIGHT, true);
    bool down  = input.isKeyDown(SDL_SCANCODE_S, true) || input.isKeyDown(SDL_SCANCODE_DOWN, true);
    bool left  = input.isKeyDown(SDL_SCANCODE_A, true) || input.isKeyDown(SDL_SCANCODE_LEFT, true);

    if (up || (input.ly < -10000 && SDL_abs(input.ly) > SDL_abs(input.lx))) {
        y -= game.SPEED;
        dir = 0;
    }
    else if (right || (input.lx > 10000 && SDL_abs(input.lx) > SDL_abs(input.ly))) {
        x += game.SPEED;
        dir = 90;
    }
    else if (down || (input.ly > 10000 && SDL_abs(input.ly) > SDL_abs(input.lx))) {
        y += game.SPEED;
        dir = 180;
    }
    else if (left || (input.lx < -10000 && SDL_abs(input.lx) > SDL_abs(input.ly))) {
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
    
    tile.x = tilex;
    tile.y = tiley;
    player.x = playerx;
    player.y = playery;
}
