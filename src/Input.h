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
        void initGamepad();
        void updateJoysticks();
        bool isKeyDown(SDL_Scancode key, bool hold);
        bool isButtonDown(SDL_GamepadButton button, bool hold);
};
