//
// Created by Margherita Donnici on 1/19/17.
//

#pragma once


class Game;

class Tutorial {

public:
    void render(const Game& game) const;
    const float tutorialEndDate = 7.f; // in seconds
    const float animationInterval = 2.f; // in seconds

private:
    const float tutorialBeginDate = 1.f; // in second
};

