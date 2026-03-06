#include "Camera.h"

/* Manage scrolling background/moving player based on keypresses and map borders. Returns the origin at which to render non-player entities */
Coordinate Camera::updateCamera(Game& game, Input& input, SDL_FRect& player) {
    /* Controls */
    bool up    = input.isKeyDown(SDL_SCANCODE_W, true) || input.isKeyDown(SDL_SCANCODE_UP, true);
    bool right = input.isKeyDown(SDL_SCANCODE_D, true) || input.isKeyDown(SDL_SCANCODE_RIGHT, true);
    bool down  = input.isKeyDown(SDL_SCANCODE_S, true) || input.isKeyDown(SDL_SCANCODE_DOWN, true);
    bool left  = input.isKeyDown(SDL_SCANCODE_A, true) || input.isKeyDown(SDL_SCANCODE_LEFT, true);

    if (!game.interacting) {
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
    }

    /* Constrain the camera to map dimensions */
    Coordinate origin;
    
    if (x >= 0 && x <= game.TILEWIDTH * game.MAPWIDTH - game.SCREENWIDTH) {
        origin.x = -x;
        player.x = (game.SCREENWIDTH - game.SPRITEWIDTH) / 2;
    }
    else {
        if (x < 0) {
            origin.x = 0;
            player.x = (game.SCREENWIDTH - game.SPRITEWIDTH) / 2 + x;
        }
        else {
            origin.x = -(game.TILEWIDTH * game.MAPWIDTH - game.SCREENWIDTH);
            player.x = (game.SCREENWIDTH - game.SPRITEWIDTH) / 2 - (game.TILEWIDTH * game.MAPWIDTH - game.SCREENWIDTH) + x;
        }
    }
    if (y >= 0 && y <= game.TILEHEIGHT * game.MAPHEIGHT - game.SCREENHEIGHT) {
        origin.y = -y;
        player.y = (game.SCREENHEIGHT - game.SPRITEHEIGHT) / 2;
    }
    else {
        if (y < 0) {
            origin.y = 0;
            player.y = (game.SCREENHEIGHT - game.SPRITEHEIGHT) / 2 + y;
        }
        else {
            origin.y = -(game.TILEHEIGHT * game.MAPHEIGHT - game.SCREENHEIGHT);
            player.y = (game.SCREENHEIGHT - game.SPRITEHEIGHT) / 2 - (game.TILEHEIGHT * game.MAPHEIGHT - game.SCREENHEIGHT) + y;
        }
    }
    
    return origin;
}

/* SDL_Render uses the top-left corner of an entity, but game logic considers entities at their middle-bottom */
float normalize(SDL_FRect rect, bool isY) {
    return (isY) ? rect.y + rect.h : rect.x + rect.w / 2;
}

/* Offsets an object's position. Use 'origin' to determine true screen position */
SDL_FRect offset(SDL_FRect rect, Coordinate origin) {
    return { rect.x + origin.x, rect.y + origin.y, rect.w, rect.h };
}
