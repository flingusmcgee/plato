#include "EntityManager.h"

/* Finds the closest entities within a rectangle above and below the player */
int EntityManager::getClosestTarget(std::vector<Entity>& order, int playeridx, int dir, float width, float height) {
    above = { };
    below = { };

    float xdist;
    float closestdist;
    int closest = -1;

    for (int i = playeridx - 1; i >= 0; --i) {
        if (normalize(order[i].rect, 1) - normalize(order[playeridx].rect, 1) <= -height) {
            break;
        }
        xdist = normalize(order[i].rect, 0) - normalize(order[playeridx].rect, 0);
        if (SDL_fabsf(xdist) <= width && (xdist * (dir - 180) < 0 || dir % 180 == 0)) {
            above.push_back(i);
            if (dir != 180 && (getDistance(order[i].rect, order[playeridx].rect) < closestdist || closest == -1)) {
                closest = i;
                closestdist = getDistance(order[i].rect, order[playeridx].rect);
            }
        }
    }
    for (int i = playeridx + 1; i <= order.size() - 1; ++i) {
        if (normalize(order[i].rect, 1) - normalize(order[playeridx].rect, 1) >= height) {
            break;
        }
        xdist = normalize(order[i].rect, 0) - normalize(order[playeridx].rect, 0);
        if (SDL_fabsf(xdist) <= width && (xdist * (dir - 180) < 0 || dir % 180 == 0)) {
            below.push_back(i);
            if (dir != 0 && (getDistance(order[i].rect, order[playeridx].rect) < closestdist || closest == -1)) {
                closest = i;
                closestdist = getDistance(order[i].rect, order[playeridx].rect);
            }
        }
    }

    return closest;
}

float EntityManager::getDistance(SDL_FRect self, SDL_FRect target) {
    const float xdist = normalize(self, 0) - normalize(target, 0);
    const float ydist = normalize(self, 1) - normalize(target, 1);
    return SDL_sqrtf(xdist * xdist + ydist * ydist);
}

float EntityManager::getDirection(SDL_FRect self, SDL_FRect target, Coordinate origin) {
    return SDL_atan2f(normalize(target, 1) - normalize(offset(self, origin), 1), normalize(target, 0) - normalize(offset(self, origin), 0));
}

SDL_FRect EntityManager::move(SDL_FRect self, float angle, float speed) {
    return offset(self, { SDL_cosf(angle) * speed, SDL_sinf(angle) * speed });
}
