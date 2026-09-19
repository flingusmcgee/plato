#include "Entity.h"

Entity::Entity(Game& game, std::string name) {
    EntityTemplate entityTemplate = game.ENTITYTEMPLATES.at(name);
    type     = entityTemplate.type;
    group    = entityTemplate.group;
    textures = entityTemplate.textures;
    prect    = entityTemplate.rect;
    hitbox   = entityTemplate.hitbox;
    hitboxLayer = entityTemplate.hitboxLayer;
    speed    = entityTemplate.speed;
    range    = entityTemplate.range;

    if (type == 1) {
        prect.x = game.SCREENWIDTH / 2;
        prect.y = game.SCREENHEIGHT / 2 + prect.h / 2;
    }
}

/* Manages movement and animations */
void Entity::updateEntity(Coordinate offset, int texture, std::vector<Entity>& order) {
    hitbox.x = prect.x - hitbox.w / 2 + offset.x;
    for (Entity entity : order) {
        if (entity.hitboxLayer <= hitboxLayer) {
            if (isColliding(entity.hitbox)) {
                hitbox.x -= offset.x;
                break;
            }
        }
    }
    hitbox.y = prect.y - hitbox.h + offset.y;
    for (Entity entity : order) {
        if (entity.hitboxLayer <= hitboxLayer) {
            if (isColliding(entity.hitbox)) {
                hitbox.y -= offset.y;
                break;
            }
        }
    }
    prect.x = getPhysicsAnchor(hitbox).x;
    prect.y = getPhysicsAnchor(hitbox).y;
    this->texture = texture;
}

void Entity::highlight(SDL_Renderer *renderer, const Game& game) {
    const float DISTANCE = 10;
    SDL_FRect highlighter = drect;
    highlighter.y += SDL_sinf((float) game.ticks / NSCOEFFICIENT * game.ANIMATIONSPEED) * DISTANCE;
    SDL_RenderTexture(renderer, game.iconList[0], NULL, &highlighter);
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
        if (up || input.ly < -JOYSTICKTHRESHOLD) {
            offset.y -= 1;
        }
        if (right || input.lx > JOYSTICKTHRESHOLD) {
            offset.x += 1;
        }
        if (down || input.ly > JOYSTICKTHRESHOLD) {
            offset.y += 1;
        }
        if (left || input.lx < -JOYSTICKTHRESHOLD) {
            offset.x -= 1;
        }
        if (offset.x != 0 || offset.y != 0) {
            dir = SDL_atan2f(offset.y, offset.x) * 180 / SDL_PI_F;
            float throttle = SDL_sqrtf(offset.x * offset.x + offset.y * offset.y);
            offset.x /= throttle;
            offset.y /= throttle;
        }
    }
    return offset;
}

/* Finds the closest entities within a circle around an entity. Can be configured to search only in a group */
int Entity::getClosestTarget(std::vector<Entity>& order, std::string group, int entityidx, float entitydir, float range) {
    inRange = { };
    float closestdist;
    int closest = -1;
    int lastidx = entityidx;
    while (lastidx > 0 && order[entityidx].prect.y - order[lastidx - 1].prect.y <= range) {
        --lastidx;
    }
    for (int i = lastidx; i < order.size(); ++i) {
        if (SDL_fabsf(order[entityidx].prect.y - order[i].prect.y) > range) {
            break;
        }
        if (order[i].group != group && !group.empty()) {
            continue;
        }
        if (i == entityidx) {
            continue;
        }
        float dist = order[entityidx].getDistance(order[i].prect);
        float dir = order[entityidx].getDirection(order[i].prect) * 180 / SDL_PI_F;
        if (dist <= range) {
            inRange.push_back(i);
            float deltadir = SDL_fabs(dir - entitydir);
            const float CONE = 40;
            if (SDL_min(deltadir, 360 - deltadir) < CONE && (dist < closestdist || closest == -1)) {
                closest = i;
                closestdist = dist;
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

int Entity::getSprite() {
    if (dir == -90) {
        return 0;
    }
    if (SDL_fabsf(dir) < 90) {
        return 1;
    }
    if (dir == 90) {
        return 2;
    }
    if (SDL_fabsf(dir) > 90) {
        return 3;
    }
    return 0;
}
