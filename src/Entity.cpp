#include "Entity.h"

Entity::Entity(Game& game, std::string name) {
    EntityTemplate entityTemplate = game.ENTITYTEMPLATES.at(name);
    type     = entityTemplate.type;
    group    = entityTemplate.group;
    textures = entityTemplate.textures;
    prect    = entityTemplate.rect;
    hitbox   = entityTemplate.hitbox;
    speed    = entityTemplate.speed;
    range    = entityTemplate.range;
    breadth  = entityTemplate.breadth;

    if (type == 1) {
        prect.x = game.SCREENWIDTH / 2;
        prect.y = game.SCREENHEIGHT / 2 + prect.h / 2;
    }
}

/* Control an entity with user input */
Coordinate Entity::getMoveControls(Game& game, Input& input) {
    const int JOYSTICKTHRESHOLD = 10000;
    bool up    = input.isKeyDown(SDL_SCANCODE_W, true) || input.isKeyDown(SDL_SCANCODE_UP, true);
    bool right = input.isKeyDown(SDL_SCANCODE_D, true) || input.isKeyDown(SDL_SCANCODE_RIGHT, true);
    bool down  = input.isKeyDown(SDL_SCANCODE_S, true) || input.isKeyDown(SDL_SCANCODE_DOWN, true);
    bool left  = input.isKeyDown(SDL_SCANCODE_A, true) || input.isKeyDown(SDL_SCANCODE_LEFT, true);

    Coordinate offset { 0, 0 };
    if (!game.interacting) {
        if (up || (input.ly < -JOYSTICKTHRESHOLD && SDL_abs(input.ly) > SDL_abs(input.lx))) {
            offset.y -= speed;
            dir = 0;
        }
        else if (right || (input.lx > JOYSTICKTHRESHOLD && SDL_abs(input.lx) > SDL_abs(input.ly))) {
            offset.x += speed;
            dir = 90;
        }
        else if (down || (input.ly > JOYSTICKTHRESHOLD && SDL_abs(input.ly) > SDL_abs(input.lx))) {
            offset.y += speed;
            dir = 180;
        }
        else if (left || (input.lx < -JOYSTICKTHRESHOLD && SDL_abs(input.lx) > SDL_abs(input.ly))) {
            offset.x -= speed;
            dir = 270;
        }
    }
    return offset;
}

/* Manages movement and animations */
void Entity::updateEntity(Coordinate offset, int texture, std::vector<Entity>& order) {
    hitbox.x = prect.x - hitbox.w / 2 + offset.x;
    for (Entity entity : order) {
        if (isColliding(entity.hitbox)) {
            hitbox.x -= offset.x;
            break;
        }
    }
    hitbox.y = prect.y - hitbox.h + offset.y;
    for (Entity entity : order) {
        if (isColliding(entity.hitbox)) {
            hitbox.y -= offset.y;
            break;
        }
    }
    prect.x = getPhysicsAnchor(hitbox).x;
    prect.y = getPhysicsAnchor(hitbox).y;
    this->texture = texture;
}

/* Finds the closest entities within a rectangle above and below an entity. Can be configured to search only in a group */
int Entity::getClosestTarget(std::vector<Entity>& order, std::string group, int entityidx, int dir, SDL_FRect bound) {
    above = { };
    below = { };

    float xdist;
    float closestdist;
    int closest = -1;

    for (int i = entityidx - 1; i >= 0; --i) {
        if (order[i].prect.y - order[entityidx].prect.y <= -bound.h / 2) {
            break;
        }
        if (order[i].group != group && !group.empty()) {
            continue;
        }
        xdist = order[i].prect.x - order[entityidx].prect.x;
        if (SDL_fabsf(xdist) <= bound.w / 2 && (xdist * (dir - 180) < 0 || dir % 180 == 0)) {
            above.push_back(i);
            if (dir != 180 && (order[i].getDistance(order[entityidx].prect) < closestdist || closest == -1)) {
                closest = i;
                closestdist = order[i].getDistance(order[entityidx].prect);
            }
        }
    }
    for (int i = entityidx + 1; i <= order.size() - 1; ++i) {
        if (order[i].prect.y - order[entityidx].prect.y >= bound.h / 2) {
            break;
        }
        if (order[i].group != group && !group.empty()) {
            continue;
        }
        xdist = order[i].prect.x - order[entityidx].prect.x;
        if (SDL_fabsf(xdist) <= bound.w / 2 && (xdist * (dir - 180) < 0 || dir % 180 == 0)) {
            below.push_back(i);
            if (dir != 0 && (order[i].getDistance(order[entityidx].prect) < closestdist || closest == -1)) {
                closest = i;
                closestdist = order[i].getDistance(order[entityidx].prect);
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
