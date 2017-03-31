// Created by Margherita Donnici on 1/3/17.

#pragma once

#include "point2.h"

enum textquality {solid, shaded, blended};
enum fontSize {big, small};

class TextRenderer {
public:

    TTF_Font *smallFont;
    TTF_Font *bigFont;
    void initialize();
    void render(SDL_Color textColor, SDL_Color backgroundColor, const char *text, Point2 position, enum textquality quality,
                    enum fontSize fontSize, bool wrap) const;

private:
    /* Maschere RGB per creare superfici RGB */
    Uint32 rmask, gmask, bmask, amask;

};


