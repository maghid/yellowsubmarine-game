#pragma once

#include <SDL2/SDL_keycode.h>
#include "InputEvent.h"

class Game;

class Input {

public:

    bool isMoveForwardPressed() const {
        return moveForwardPressed;
    }

    bool isMoveBackwardPressed() const {
        return moveBackwardPressed;
    }

    bool isUpKeyPressed() const {
        return upKeyPressed;
    }

    bool isLeftKeyPressed() const {
        return leftKeyPressed;
    }

    bool isRightKeyPressed() const {
        return rightKeyPressed;
    }

    bool isDownKeyPressed() const {
        return downKeyPressed;
    }

    void KeyCodeToInputEvent(SDL_Keycode keycode, InputEvent &outInputEvent, bool &isEventFound);


    void onInputEvent(InputEvent inputEvent, Game &game);

    void EatKey(SDL_Keycode keycode, bool pressed, bool joystick, Game &game);

    void EatJoyAxis(int axisIndex, int axisValue);

private:
    // Controlli per il movimento del sottomarino
    bool moveForwardPressed = false;
    bool moveBackwardPressed = false;
    bool leftKeyPressed = false;
    bool rightKeyPressed = false;
    bool upKeyPressed = false;
    bool downKeyPressed = false;
};
