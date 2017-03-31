// Created by Margherita Donnici on 1/7/17.

#pragma once

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <array>
#include "InputEvent.h"
#include "point2.h"

class Game;


enum class MenuState {graphicsMenu, gameOver, none};

class MenuManager {
public:
    void render(const Game& game) const;
    void setMenuState(MenuState state);
    bool capturesInput() const;
    void onInputEvent(InputEvent inputEvent, Game& game);
private:

    MenuState currentMenuState = MenuState::none;
    void renderGraphicsMenu(const Game& game) const;
    void renderGameOver(const Game& game) const;

    // Strutture dati per il menù
    struct menuElement {
        std::string string;
        Point2 position;
        menuElement(const std::string &string, const Point2 &position) : string(string), position(position) {}

    };

    std::array<std::string,5> menuStrings ={ {"F1   Help",
                                  "F2   Cambia visuale",
                                  "F3   Faro",
                                  "F4   Riflessi dell'acqua",
                                  "F5   Ombre",
                                }};
};
