#include "Input.h"

void Input::initKeyboard() {
    if (keyJustDown.empty()) {
        int numkeys;
        SDL_GetKeyboardState(&numkeys);
        keyJustDown.resize(numkeys);
    }
}

void Input::initGamepad() {
    if (buttonJustDown.empty() && gamepad) {
        int numbuttons;
        SDL_GetGamepadBindings(gamepad, &numbuttons);
        buttonJustDown.resize(numbuttons);
    }
}

void Input::updateJoysticks() {
    lx = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
    ly = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);
    rx = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
    ry = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHTY);
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
bool Input::isButtonDown(SDL_GamepadButton buttonIn, bool hold) {
    bool output;
    bool button = SDL_GetGamepadButton(gamepad, buttonIn);
    output = (hold) ? button : buttonJustDown[buttonIn] && button;
    if (button && buttonJustDown[buttonIn]) {
        buttonJustDown[buttonIn] = false;
    }
    if (!button && !buttonJustDown[buttonIn]) {
        buttonJustDown[buttonIn] = true;
    }
    return output;
}
