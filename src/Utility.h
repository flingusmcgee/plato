#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <string_view>
#include <vector>
#include <format>
#include <map>

const Uint32 NSCOEFFICIENT = 1'000'000'000;

struct Vector {
    float x;
    float y;

    Vector operator+(Vector vector) {
        return { this->x + vector.x, this->y + vector.y };
    }
    Vector operator*(float scalar) {
        return { this->x * scalar, this->y * scalar };
    }
};

struct Text {
    std::vector<SDL_Texture *> texture;
    std::vector<std::string> data;
    std::vector<float> w;
    float h;
    Uint64 reftick;
};

SDL_FRect getRenderAnchor(SDL_FRect rect);
SDL_FRect getPhysicsAnchor(SDL_FRect rect);
SDL_FRect translate(SDL_FRect rect, Vector offset);
Vector getVector(float angle, float magnitude);

template <typename T>
bool isEmpty(std::vector<std::vector<T>> container, const char *messageTotal, const char *messageComponent);
template <typename T>
bool isEmpty(T container, const char *message);

/**
 * Flexible emptiness check for safe handling - Multi-Dimensional Edition
 * \param container multi-dimensional data structure to be tested
 * \param messageTotal log message on total emptiness
 * \param messageComponent log message on individual emptiness
 */
template <typename T>
bool isEmpty(std::vector<std::vector<T>> container, const char *messageTotal, const char *messageComponent) {
    bool value = isEmpty(container, messageTotal);
    if (!value) {
        int i = 0;
        for (auto row : container) {
            if (row.empty()) {
                SDL_Log("%s %d", messageComponent, i);
                value = true;
                i++;
            }
        }
    }
    return value;
}
/**
 * Flexible emptiness check for safe handling
 * \param container data structure to be tested
 * \param message log message on total emptiness
 */
template <typename T>
bool isEmpty(T container, const char *message) {
    if (container.empty()) {
        SDL_Log("%s", message);
        return true;
    }
    return false;
}
