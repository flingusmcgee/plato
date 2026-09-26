#include "Entity.h"

Entity::Entity(const Game& game, std::string name) {
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
void Entity::updateEntity(Vector offset, int texture) {
    hitbox.x = offset.x;
    hitbox.y = offset.y;
    prect.x = getPhysicsAnchor(hitbox).x;
    prect.y = getPhysicsAnchor(hitbox).y;
    this->texture = texture;
}

/* Tests an offset against collisions and applies it to displacement */
void Entity::applyDisplacement(Vector offset, const std::vector<Entity>& order) {
    SDL_FRect originHitbox = hitbox;

    hitbox.x = prect.x - hitbox.w / 2 + offset.x;
    for (Entity entity : order) {
        if (entity.hitboxLayer <= hitboxLayer) {
            if (isColliding(entity.hitbox)) {
                hitbox.x = originHitbox.x;
                break;
            }
        }
    }
    hitbox.y = prect.y - hitbox.h + offset.y;
    for (Entity entity : order) {
        if (entity.hitboxLayer <= hitboxLayer) {
            if (isColliding(entity.hitbox)) {
                hitbox.y = originHitbox.y;
                break;
            }
        }
    }

    displacement = { hitbox.x, hitbox.y };
    hitbox = originHitbox;
}

void Entity::highlight(SDL_Renderer *renderer, const Game& game) {
    const float DISTANCE = 10;
    SDL_FRect highlighter = drect;
    highlighter.y += SDL_sinf((float) game.ticks / NSCOEFFICIENT * game.ANIMATIONSPEED) * DISTANCE;
    SDL_RenderTexture(renderer, game.iconList[0], NULL, &highlighter);
}

/* Finds the closest entities within a circle around an entity. Can be configured to search only in a group */
int Entity::getClosestTarget(const std::vector<Entity>& order, std::string group, int entityidx, float entitydir, float range) {
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

int Entity::getSprite() const {
    if (dir == 0) {
        return 1;
    }
    if (dir == 180) {
        return 3;
    }
    if (dir > 0) {
        return 2;
    }
    if (dir < 0) {
        return 0;
    }
    return 0;
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