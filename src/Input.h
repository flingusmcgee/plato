#pragma once
#include "Game.h"

class Input {
    public:
        SDL_Gamepad *gamepad;
        const bool *code;
        std::vector<bool> keyJustDown;
        std::vector<bool> buttonJustDown;

        short lx;
        short ly;
        short rx;
        short ry;

        void initKeyboard();
        void initGamepad(SDL_Gamepad *gamepadIn);
        void updateJoysticks();
        bool isKeyDown(SDL_Scancode key, bool hold);
        bool isButtonDown(SDL_GamepadButton buttonIn, bool hold);
};
