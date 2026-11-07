#include "Utility.h"

/* Get the coordinates of the top-left corner required for `SDL_Render`. Inverse of `getPhysicsAnchor` */
SDL_FRect getRenderAnchor(SDL_FRect rect) {
    return { rect.x - rect.w / 2, rect.y - rect.h, rect.w, rect.h };
}

/* Get the coordinates of the bottom-middle required for physics. Inverse of `getRenderAnchor` */
SDL_FRect getPhysicsAnchor(SDL_FRect rect) {
    return { rect.x + rect.w / 2, rect.y + rect.h, rect.w, rect.h };
}

/* Translates a rect's x and y coordinates */
SDL_FRect translate(SDL_FRect rect, Vector offset) {
    return { rect.x + offset.x, rect.y + offset.y, rect.w, rect.h };
}

/* Converts a movement vector into a translatable offset value */
Vector getVector(float angle, float magnitude) {
    return { SDL_cosf(angle) * magnitude, SDL_sinf(angle) * magnitude };
}
