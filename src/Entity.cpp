#include "Entity.h"

void Entity::updateEntity(Coordinate offset, int texture) {
    prect = translate(prect, offset);
    hitbox.x = prect.x;
    hitbox.y = prect.y;
    hitbox = getRenderAnchor(hitbox);
    this->texture = texture;
}

/* Finds the closest entities within a rectangle above and below the player. Can be configured to search only in a group */
int Entity::getClosestTarget(std::vector<Entity>& order, std::string group, int playeridx, int dir, float width, float height) {
    above = { };
    below = { };

    float xdist;
    float closestdist;
    int closest = -1;

    for (int i = playeridx - 1; i >= 0; --i) {
        if (order[i].prect.y - order[playeridx].prect.y <= -height) {
            break;
        }
        if (order[i].group != group && !group.empty()) {
            continue;
        }
        xdist = order[i].prect.x - order[playeridx].prect.x;
        if (SDL_fabsf(xdist) <= width && (xdist * (dir - 180) < 0 || dir % 180 == 0)) {
            above.push_back(i);
            if (dir != 180 && (order[i].getDistance(order[playeridx].prect) < closestdist || closest == -1)) {
                closest = i;
                closestdist = order[i].getDistance(order[playeridx].prect);
            }
        }
    }
    for (int i = playeridx + 1; i <= order.size() - 1; ++i) {
        if (order[i].prect.y - order[playeridx].prect.y >= height) {
            break;
        }
        if (order[i].group != group && !group.empty()) {
            continue;
        }
        xdist = order[i].prect.x - order[playeridx].prect.x;
        if (SDL_fabsf(xdist) <= width && (xdist * (dir - 180) < 0 || dir % 180 == 0)) {
            below.push_back(i);
            if (dir != 0 && (order[i].getDistance(order[playeridx].prect) < closestdist || closest == -1)) {
                closest = i;
                closestdist = order[i].getDistance(order[playeridx].prect);
            }
        }
    }

    return closest;
}

float Entity::getDistance(SDL_FRect target) {
    const float xdist = prect.x - target.x;
    const float ydist = prect.y - target.y;
    return SDL_sqrtf(xdist * xdist + ydist * ydist);
}

float Entity::getDirection(SDL_FRect target) {
    return SDL_atan2f(target.y - prect.y, target.x - prect.x);
}

void Entity::highlight(SDL_Renderer *renderer, const Game& game) {
    const float SPEED = 300;
    const float DISTANCE = 10;
    SDL_FRect highlighter = drect;
    highlighter.y += SDL_sinf((float) game.ticks / SPEED) * DISTANCE;
    SDL_RenderTexture(renderer, game.iconList[0], NULL, &highlighter);
}

bool Entity::isColliding(SDL_FRect target) {
    const float SELFTOP = hitbox.y;
    const float SELFBOTTOM = hitbox.y + hitbox.h;
    const float SELFLEFT = hitbox.x;
    const float SELFRIGHT = hitbox.x + hitbox.w;
    const float TARGETTOP = target.y;
    const float TARGETBOTTOM = target.y + target.h;
    const float TARGETLEFT = target.x;
    const float TARGETRIGHT = target.x + target.w;

    return SELFTOP < TARGETBOTTOM && SELFBOTTOM > TARGETTOP && SELFRIGHT > TARGETLEFT && SELFLEFT < TARGETRIGHT;
}
