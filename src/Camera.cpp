#include "Camera.h"
#include "Game.h"

/* Track a position confined to map borders. */
void Camera::updateCamera(const Game& game, const SDL_FRect& prect) {
    float x = prect.x - game.SCREENWIDTH / 2;
    float y = prect.y - game.SCREENHEIGHT / 2 - prect.h / 2;

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
SDL_FRect Camera::offsetToCamera(SDL_FRect rect) const {
    return translate(rect, origin);
}

void Camera::shakeCamera(int strength, int tick) {
    if (tick - reftick < 2000) {
        if ((tick - reftick) % 1 == 0) {
            origin.x -= shakeOffset.x;
            origin.y -= shakeOffset.y;
            shakeOffset.x = SDL_rand(strength * 2) - strength;
            shakeOffset.y = SDL_rand(strength * 2) - strength;
            origin = origin + shakeOffset;
        }
    }
    else {
        origin.x -= shakeOffset.x;
        origin.y -= shakeOffset.y;
        shakeOffset = { 0, 0 };
    }
}
