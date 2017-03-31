// Created by Margherita Donnici on 12/27/16.

#pragma once

#include "point3.h"

class Point2;

namespace utils {
    /* Restituisce un float aleatorio compreso tra 0 ed 1 */
    float random_float();

    float get_elapsed_seconds();

    float distance(const Point3& A, const Point3& B);

    float radiansToDegrees(const float angleRadians);

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord
// del pixel sullo schermo
    void setCoordToPixel(const Point2& screenSize);
	GLenum checkGLError(const char* file, int line);
}
