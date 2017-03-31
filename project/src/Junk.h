// Created by Margherita Donnici on 12/5/16.

#pragma once

#include "point3.h"

class Junk {
public:
    Junk(const Point3 &initialPosition,
         const float spawnTime,
         const float fallSpeed,
         const Vector3 &rotation);
    void render(const float gameSeconds, bool useShadow) const; // Disegna detriti a schermo
    Point3 getPosition(const float gameSeconds) const;
    float getRadius() const {
        return 1.f;
    }
    Vector3 rotation; // angoli di Eulero, in gradi

private:

    Point3 initialPosition;
    float spawnTime; // in seconds
    float fallSpeed;

};
