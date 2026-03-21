#include "Camera.h"

/* Manage scrolling background/moving player based on keypresses and map borders. Returns the origin at which to render non-player entities */
void Camera::updateCamera(Game& game, Input& input, SDL_FRect& prect, float speed) {
    /* Controls */
    const int JOYSTICKTHRESHOLD = 10000;
    bool up    = input.isKeyDown(SDL_SCANCODE_W, true) || input.isKeyDown(SDL_SCANCODE_UP, true);
    bool right = input.isKeyDown(SDL_SCANCODE_D, true) || input.isKeyDown(SDL_SCANCODE_RIGHT, true);
    bool down  = input.isKeyDown(SDL_SCANCODE_S, true) || input.isKeyDown(SDL_SCANCODE_DOWN, true);
    bool left  = input.isKeyDown(SDL_SCANCODE_A, true) || input.isKeyDown(SDL_SCANCODE_LEFT, true);

    if (!game.interacting) {
        if (up || (input.ly < -JOYSTICKTHRESHOLD && SDL_abs(input.ly) > SDL_abs(input.lx))) {
            y -= speed;
            dir = 0;
        }
        else if (right || (input.lx > JOYSTICKTHRESHOLD && SDL_abs(input.lx) > SDL_abs(input.ly))) {
            x += speed;
            dir = 90;
        }
        else if (down || (input.ly > JOYSTICKTHRESHOLD && SDL_abs(input.ly) > SDL_abs(input.lx))) {
            y += speed;
            dir = 180;
        }
        else if (left || (input.lx < -JOYSTICKTHRESHOLD && SDL_abs(input.lx) > SDL_abs(input.ly))) {
            x -= speed;
            dir = 270;
        }
    }

    prect.x = x + game.SCREENWIDTH / 2;
    prect.y = y + game.SCREENHEIGHT / 2 + prect.h / 2;

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
    return translate(rect, translate(origin, shakeOffset));
}

void Camera::shakeCamera(int strength, int tick) {
    if (tick - reftick < 200) {
        if ((tick - reftick) % 50 == 0) {
            shakeOffset.x = SDL_rand(strength * 2) - strength;
            shakeOffset.y = SDL_rand(strength * 2) - strength;
        }
    }
    else {
        shakeOffset = { 0, 0 };
    }
}

/* Get the coordinates of the top-left corner required for SDL_Render. Inverse of 'getPhysicsAnchor' */
SDL_FRect getRenderAnchor(SDL_FRect rect) {
    return { rect.x - rect.w / 2, rect.y - rect.h, rect.w, rect.h };
}

/* Get the coordinates of the bottom-middle required for physics. Inverse of 'getRenderAnchor' */
SDL_FRect getPhysicsAnchor(SDL_FRect rect) {
    return { rect.x + rect.w / 2, rect.y + rect.h, rect.w, rect.h };
}

/* Translates a rect's x and y coordinates */
SDL_FRect translate(SDL_FRect rect, Coordinate offset) {
    return { rect.x + offset.x, rect.y + offset.y, rect.w, rect.h };
}

/* Translates a point's x and y coordinates */
Coordinate translate(Coordinate point, Coordinate offset) {
    return { point.x + offset.x, point.y + offset.y };
}

/* Converts a movement vector into a translatable offset value */
Coordinate getMoveOffset(float angle, float speed) {
    return { SDL_cosf(angle) * speed, SDL_sinf(angle) * speed };
}

/* Accepts the render rectangle (drect) of the target */
bool isInView(SDL_FRect rect, Game& game) {
    return rect.x > -rect.w && rect.x < game.SCREENWIDTH && rect.y > -rect.h && rect.y < game.SCREENHEIGHT;
}
