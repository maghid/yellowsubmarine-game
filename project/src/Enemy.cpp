// Created by Margherita Donnici on 12/29/16.

#include "Enemy.h"
#include "Physics.h"
#include "Utils.h"
#include "Game.h"

// Costruttore
Enemy::Enemy(const Point3 &position, float speed, float scale,
             const Vector3 &rotation, bool rotatesTowardsPlayer,
             bool isAnimated, const Mesh *const enemyMesh, GLuint texture)
        : position(position), speed(speed),
          rotatesTowardsPlayer(rotatesTowardsPlayer), isAnimated(isAnimated),
          enemyMesh(enemyMesh), scale(scale), textureID(texture) {}

void Enemy::render(const Game &game) const {

    // Render Enemy
    glPushMatrix();
    {
        glEnable(GL_LIGHTING);
        // Mi preparo per il rendering del nemico (posizione, scala e rotazione)

        glTranslate(position);
        if (isAnimated) {
            glScalef(1.f, (cosf(game.getGameSeconds()) + 4.f) / 4.f, 1.f); // movimento ondulatorio sull'asse Y per le meduse
        }
        glScalef(scale, scale, scale);
        glRotatef(rotation.X(), 1.f, 0.f, 0.f); // rotazione intorno all'asse X
        glRotatef(rotation.Y(), 0.f, 1.f, 0.f); // rotazione intorno all'asse Y
        glRotatef(rotation.Z(), 0.f, 0.f, 1.f); // rotazione intorno all'asse Z

        // Attivo la texture

        glBindTexture(GL_TEXTURE_2D, textureID);
        glEnable(GL_TEXTURE_2D);
        assert(enemyMesh);
        // Rendering della mesh
        enemyMesh->RenderNxV(/*useTextureCoords*/ true);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
    }
    glPopMatrix();

    // Render Shadow
    if (game.useShadow) {
        glPushMatrix();
        {
            glTranslatef(position.X(), 0.35f, position.Z()); // Segue il nemico ma rimanendo al livello del terreno sull'asse delle Y

            /* La scala viene moltiplicata per un parametro che dipende dalla posizione del nemico vero e proprio, per fare in modo
             che più si avvicina al terreno più l'ombre si ingrandisce */
            glScalef(scale * (1 - (position.Y() / 40.f)),
                     scale * (1 - position.Y() / 40.f),
                     scale * (1 - (position.Y() / 40.f)));

            glScalef(1.01f, 0.f, 1.01f); // Appiattisco sulla Y, ingrandisco dell' 1% sulla X e la Z

            glRotatef(rotation.X(), 1.f, 0.f, 0.f); // rotazione intorno all'asse X
            glRotatef(rotation.Y(), 0.f, 1.f, 0.f); // rotazione intorno all'asse Y
            glRotatef(rotation.Z(), 0.f, 0.f, 1.f); // rotazione intorno all'asse Z

            glColor3f(12.f / 255.f, 12.f / 255.f, 12.f / 255.f); // Colore fisso

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            enemyMesh->RenderNxV(/*useTextureCoords*/ false);

            glDisable(GL_BLEND);
            glColor3f(1.f, 1.f, 1.f);
        }
        glPopMatrix();
    }
    utils::checkGLError(__FILE__, __LINE__);
}

void Enemy::doStep(const Submarine &sub) {

    /* CALCOLA NUOVA POSIZIONE */
    {
        const Point3 subCoord = sub.getPosition();
        // Calcolo il vettore dal nemico al sottomarino
        const Vector3 vectorToSub = subCoord - position;
        const float subToEnemyDistance = vectorToSub.modulo();
        // Calcolo la distanza che il nemico percorre in un step di fisica
        // Controllando che non si vada troppo lontano
        const float distanceToTravel =
                fminf(speed * Physics::stepSeconds, subToEnemyDistance);
        // Calcolo il vettore che descrive lo spostamento del nemico in uno step
        const Vector3 travelVector = vectorToSub.NormalizeSafe() * distanceToTravel;
        position = position + travelVector;
    }
    /* CALCOLA NUOVA ROTAZIONE */
    if (rotatesTowardsPlayer) {
        const Vector3 vectorToSub = sub.getPosition() - position;
        /* uso atan2f per calcolare l'angolo con l'asse Y*/
        const float facingRadiansY = atan2f(vectorToSub.X(), vectorToSub.Z());
        const float facingDegreesY =
                utils::radiansToDegrees(facingRadiansY); // openGL usa i gradi
        rotation = Vector3{0.f, facingDegreesY, 0.f};
    }
}
