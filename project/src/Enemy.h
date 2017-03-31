// Created by Margherita Donnici on 12/29/16.

#pragma once


#include "point3.h"
#include "Submarine.h"
#include "mesh.h"
#include "Textures.h"
#include <memory>

class Game;

class Enemy {
public:
    Enemy(const Point3 &position, float speed, float scale, const Vector3 &rotation, bool rotatesTowardsPlayer,
          bool isAnimated,
          const Mesh *enemyMesh, GLuint textureName);

    /* Restituisce la posizione del nemico */
    Point3 getPosition() const {
        return position;
    }

    /* Restituisce il raggio di azione del nemico */
    float getRadius() const {
        return 0.5f;
    }

    /* Resa grafica del nemico */
    void render(const Game &game) const;

    /* Esegue un passo di fisica */
    void doStep(const Submarine &sub);

    Vector3 rotation; // angoli di Eulero, in gradi, per calcolare la rotazione del nemico

private:
    Point3 position;
    float speed; // in metri al secondo
    bool rotatesTowardsPlayer; // true se il nemico è rivolto verso il giocatore
    bool isAnimated; // true se il nemico è animato (per le meduse)
    const Mesh *enemyMesh; // must not be null
    float scale; // scala per la dimensione del nemico
    GLuint textureID;

};
