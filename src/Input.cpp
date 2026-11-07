#include "Input.h"
#include "Game.h"

void Input::initKeyboard() {
    if (keyJustDown.empty()) {
        int numkeys;
        SDL_GetKeyboardState(&numkeys);
        keyJustDown.resize(numkeys);
    }
}

void Input::initGamepad(SDL_Gamepad *gamepad) {
    this->gamepad = gamepad;
    if (buttonJustDown.empty() && this->gamepad) {
        int numbuttons;
        SDL_GetGamepadBindings(this->gamepad, &numbuttons);
        buttonJustDown.resize(numbuttons);
    }
}

void Input::updateJoysticks() {
    if (gamepad) {
        lx = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
        ly = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);
        rx = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
        ry = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY);
    }
    else {
        lx = 0;
        ly = 0;
        rx = 0;
        ry = 0;
    }
}

/* Get keyboard key press with options for holding or single stroke */
bool Input::isKeyDown(SDL_Scancode key, bool hold) {
    bool output;
    code = SDL_GetKeyboardState(NULL);
    output = (hold) ? code[key] : keyJustDown[key] && code[key];
    if (code[key] && keyJustDown[key]) {
        keyJustDown[key] = false;
    }
    if (!code[key] && !keyJustDown[key]) {
        keyJustDown[key] = true;
    }
    return output;
}

/* Get gamepad button press with options for holding or single stroke */
bool Input::isButtonDown(SDL_GamepadButton button, bool hold) {
    bool output;
    if (gamepad) {
        bool buttonDown = SDL_GetGamepadButton(gamepad, button);
        output = (hold) ? buttonDown : buttonJustDown[button] && buttonDown;
        if (buttonDown && buttonJustDown[button]) {
            buttonJustDown[button] = false;
        }
        if (!buttonDown && !buttonJustDown[button]) {
            buttonJustDown[button] = true;
        }
    }
    return output;
}

/* Get a movement vector and set direction from user input */
Vector Input::getMoveControls(const Game& game) {
    const int JOYSTICKTHRESHOLD = 10000;
    bool up    = isKeyDown(SDL_SCANCODE_W, true) || isKeyDown(SDL_SCANCODE_UP, true);
    bool right = isKeyDown(SDL_SCANCODE_D, true) || isKeyDown(SDL_SCANCODE_RIGHT, true);
    bool down  = isKeyDown(SDL_SCANCODE_S, true) || isKeyDown(SDL_SCANCODE_DOWN, true);
    bool left  = isKeyDown(SDL_SCANCODE_A, true) || isKeyDown(SDL_SCANCODE_LEFT, true);

    Vector offset { 0, 0 };
    if (!game.interacting) {
        if (up || ly < -JOYSTICKTHRESHOLD) {
            offset.y -= 1;
        }
        if (right || lx > JOYSTICKTHRESHOLD) {
            offset.x += 1;
        }
        if (down || ly > JOYSTICKTHRESHOLD) {
            offset.y += 1;
        }
        if (left || lx < -JOYSTICKTHRESHOLD) {
            offset.x -= 1;
        }
        if (offset.x != 0 || offset.y != 0) {
            float throttle = SDL_sqrtf(offset.x * offset.x + offset.y * offset.y);
            offset.x /= throttle;
            offset.y /= throttle;
        }
    }
    return offset;
}