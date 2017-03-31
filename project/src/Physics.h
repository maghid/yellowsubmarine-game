// Created by Margherita Donnici on 12/29/16.

#pragma once
#include <SDL2/SDL_hints.h>

namespace Physics {
    constexpr Uint32 stepMs = 10; // numero di millisec che un passo di fisica simula
    constexpr float stepSeconds = stepMs / 1000.f;

    constexpr Uint32 fpsSampling = 3000; // lunghezza intervallo di calcolo fps
};
