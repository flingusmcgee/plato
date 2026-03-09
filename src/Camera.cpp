#include "Camera.h"

/* Manage scrolling background/moving player based on keypresses and map borders. Returns the origin at which to render non-player entities */
void Camera::updateCamera(Game& game, Input& input, SDL_FRect& prect) {
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

    prect.x = x + game.SCREENWIDTH / 2;
    prect.y = y + game.SCREENHEIGHT / 2 + game.SPRITEHEIGHT / 2;

    /* Constrain the camera to map dimensions */
    const int XCONSTRAINT = game.TILEWIDTH * game.MAPWIDTH - game.SCREENWIDTH;
    if (x >= 0 && x <= XCONSTRAINT) {
        origin.x = -x;
    }
    else {
        if (x < 0) {
            origin.x = 0;
        }
        else {
            origin.x = -XCONSTRAINT;
        }
    }
    const int YCONSTRAINT = game.TILEHEIGHT * game.MAPHEIGHT - game.SCREENHEIGHT;
    if (y >= 0 && y <= YCONSTRAINT) {
        origin.y = -y;
    }
    else {
        if (y < 0) {
            origin.y = 0;
        }
        else {
            origin.y = -YCONSTRAINT;
        }
    }
}

/* Offsets an object's position to camera origin */
SDL_FRect Camera::offset(SDL_FRect rect) {
    return translate(rect, origin);
}

/* Game logic considers entities at their middle-bottom, but the top-left is required for SDL_Render */
SDL_FRect getRenderAnchor(SDL_FRect rect) {
    return { rect.x - rect.w / 2, rect.y - rect.h, rect.w, rect.h };
}

/* Inverse of 'getRenderAnchor' */
SDL_FRect getPhysicsAnchor(SDL_FRect rect) {
    return { rect.x + rect.w / 2, rect.y + rect.h, rect.w, rect.h };
}

/* Translates a rect's x and y coordinates */
SDL_FRect translate(SDL_FRect rect, Coordinate offset) {
    return { rect.x + offset.x, rect.y + offset.y, rect.w, rect.h };
}
