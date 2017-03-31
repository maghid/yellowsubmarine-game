#include <SDL2/SDL_events.h>
#include "Input.h"
#include "GameState.h"
#include "Game.h"

void Input::EatKey(SDL_Keycode keycode, bool pressed, bool joystick, Game &game) {
    if (pressed) {
        if (!joystick) {
            switch (keycode) {
                case (SDLK_w):
                    moveForwardPressed = true;
                    break;
                case (SDLK_s):
                    moveBackwardPressed = true;
                    break;
                case SDLK_a:
                    leftKeyPressed = true;
                    break;
                case SDLK_d:
                    rightKeyPressed = true;
                    break;
                case SDLK_q:
                    upKeyPressed = true;
                    break;
                case SDLK_e:
                    downKeyPressed = true;
                    break;
                default: {
                    bool HasFoundEvent = false;
                    InputEvent Event;
                    KeyCodeToInputEvent(/* in*/ keycode,/*out*/ Event,  /*out*/ HasFoundEvent);
                    if (HasFoundEvent) {
                        onInputEvent(Event, game);
                    }
                    break;
                }
            }
        } else {
            switch (keycode) {
                case 0:
                    downKeyPressed = true;
                    break;
                case 1:
                    upKeyPressed = true;
                    break;
                case 2:
                    onInputEvent(InputEvent::TurboButtonPressed, game);
                    break;
            }
        }
    }

    if (!pressed) {
        if (!joystick) {
            switch (keycode) {
                case SDLK_w:
                    moveForwardPressed = false;
                    break;
                case SDLK_s:
                    moveBackwardPressed = false;
                    break;
                case SDLK_a:
                    leftKeyPressed = false;
                    break;
                case SDLK_d:
                    rightKeyPressed = false;
                    break;
                case SDLK_q:
                    upKeyPressed = false;
                    break;
                case SDLK_e:
                    downKeyPressed = false;
                    break;
            }
        } else {
            switch (keycode) {
                case 0:
                    downKeyPressed = false;
                    break;
                case 1:
                    upKeyPressed = false;
                    break;
            }
        }
    }

}

void Input::onInputEvent(InputEvent inputEvent, Game &game) {
    if (game.menuManager.capturesInput()) {
        game.menuManager.onInputEvent(inputEvent, game);
    } else {
        if (game.getCurrentState() == GameState::Playing) {
            game.sub.onInputEvent(inputEvent, game);

        }
    }
}

void Input::KeyCodeToInputEvent(SDL_Keycode keycode, InputEvent &outInputEvent, bool &isEventFound) {
    switch (keycode) {
        case SDLK_SPACE:
            outInputEvent = InputEvent::TurboButtonPressed;
            isEventFound = true;
            break;
        case SDLK_ESCAPE:
            outInputEvent = InputEvent::QuitMenu;
            isEventFound = true;
            break;
        case SDLK_F1:
            outInputEvent = InputEvent::ToggleHelpMenu;
            isEventFound = true;
            break;

        case SDLK_F2:
            outInputEvent = InputEvent::SwitchCamera;
            isEventFound = true;
            break;

        case SDLK_F3:
            outInputEvent = InputEvent::UseHeadlight;
            isEventFound = true;
            break;

        case SDLK_F4:
            outInputEvent = InputEvent::ToggleCaustics;
            isEventFound = true;
            break;

        case SDLK_F5:
            outInputEvent = InputEvent::UseShadows;
            isEventFound = true;
            break;

        default:
            isEventFound = false;
            break;

    }
}

void Input::EatJoyAxis(int axisIndex, int axisValue) {

    if (axisIndex == 0) {
        if (axisValue < -30000) {
            leftKeyPressed = true;
            rightKeyPressed = false;
        }
        if (axisValue > 30000) {
            leftKeyPressed = false;
            rightKeyPressed = true;
        }
        if (axisValue >= -30000 && axisValue <= 30000) {
            leftKeyPressed = false;
            rightKeyPressed = false;
        }
    }

    if (axisIndex == 2) {
        if (axisValue > 0) {
            moveBackwardPressed= true;
            moveForwardPressed = false;

        } else {
            moveBackwardPressed = false;
            moveForwardPressed = false;
        }
    }
    if (axisIndex == 5) {
        if (axisValue > 0) {
            moveForwardPressed= true;
           moveBackwardPressed = false;
        } else {
           moveBackwardPressed = false;
            moveForwardPressed = false;
        }
    }
}
