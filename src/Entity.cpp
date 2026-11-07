#include "Entity.h"
#include "Camera.h"

Entity::Entity(const EntityTemplate& base, Vector spawnpos) {
    type     = base.type;
    group    = base.group;
    textures = base.textures;
    prect    = base.rect;
    hitbox   = base.hitbox;
    hitboxLayer = base.hitboxLayer;
    speed    = base.speed;
    range    = base.range;

    prect = translate(prect, spawnpos);
}

/* Manages movement and animations */
void Entity::updateEntity(const Camera& camera) {
    prect = translate(prect, displacement);
    hitbox.x = prect.x - hitbox.w / 2;
    hitbox.y = prect.y - hitbox.h;
    displacement = { 0, 0 };
    setRenderPosition(camera);
}

/* Tests an offset against collisions and applies it to displacement */
void Entity::applyDisplacement(Vector offset, const std::vector<Entity*> order) {
    if (offset.x != 0 || offset.y != 0) {
        SDL_FRect originalHitbox = hitbox;
        displacement = displacement + offset;
        
        hitbox.x = prect.x - hitbox.w / 2 + displacement.x;
        for (Entity* entity : order) {
            if (this == entity) {
                continue;
            }
            if (entity->hitboxLayer <= hitboxLayer) {
                if (isColliding(entity->hitbox)) {
                    displacement.x -= offset.x;
                    hitbox.x = originalHitbox.x;
                    break;
                }
            }
        }
        hitbox.y = prect.y - hitbox.h + displacement.y;
        for (Entity* entity : order) {
            if (this == entity) {
                continue;
            }
            if (entity->hitboxLayer <= hitboxLayer) {
                if (isColliding(entity->hitbox)) {
                    displacement.y -= offset.y;
                    hitbox.y = originalHitbox.y;
                    break;
                }
            }
        }
        
        dir = SDL_atan2f(offset.y, offset.x);
        hitbox = originalHitbox;
    }
}

void Entity::setRenderPosition(const Camera& camera) {
    drect = camera.offsetToCamera(getRenderAnchor(prect));
}

void Entity::setSprite() {
    if (dir == 0) {
        texture = 1; /* East */
    }
    else if (dir == SDL_PI_F) {
        texture = 3; /* West */
    }
    else if (dir > 0) {
        texture = 2; /* South */
    }
    else {
        texture = 0; /* North */
    }
}

/* Finds the entities within a circle around an entity and returns the closest. Can be configured to search only in a group */
int Entity::getClosestTarget(const std::vector<Entity*> order, std::string group, int currentidx, float currentdir, float range) {
    inRange = { };
    float closestDist;
    int closest = -1;
    int lastidx = currentidx;
    while (lastidx > 0 && order[currentidx]->prect.y - order[lastidx - 1]->prect.y <= range) {
        --lastidx;
    }
    for (int i = lastidx; i < order.size(); ++i) {
        if (SDL_fabsf(order[currentidx]->prect.y - order[i]->prect.y) > range) {
            break;
        }
        if (order[i]->group != group && !group.empty()) {
            continue;
        }
        if (i == currentidx) {
            continue;
        }
        float distToTarget = order[currentidx]->getDistance(order[i]->prect);
        float dirToTarget = order[currentidx]->getDirection(order[i]->prect);
        if (distToTarget <= range) {
            inRange.push_back(i);
            float dirDifference = SDL_fabs(dirToTarget - currentdir);
            const float SEARCHCONE = SDL_PI_F / 4;
            if (SDL_min(dirDifference, 2 * SDL_PI_F - dirDifference) < SEARCHCONE && (distToTarget < closestDist || closest == -1)) {
                closest = i;
                closestDist = distToTarget;
            }
        }
    }

    return closest;
}

float Entity::getDistance(SDL_FRect target) const {
    const float xdist = prect.x - target.x;
    const float ydist = prect.y - target.y;
    return SDL_sqrtf(xdist * xdist + ydist * ydist);
}

float Entity::getDirection(SDL_FRect target) const {
    return SDL_atan2f(target.y - prect.y, target.x - prect.x);
}

bool Entity::isColliding(SDL_FRect target) const {
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

Vector Entity::getChaseVector(Entity target) const {
    const float APPROACHRANGE = 150.0f;
    float speedScale = SDL_min(getDistance(target.prect) / APPROACHRANGE, 1); /* Move further objects faster */
    float angle = getDirection(target.prect);
    if (speedScale < 0.5) {
        speedScale = 0;
    }
    return getVector(angle, speed * speedScale);
}