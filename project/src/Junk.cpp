// Created by Margherita Donnici on 12/5/16.
#include "mesh.h"
#include "Junk.h"
#include "Textures.h"
#include "Utils.h"
#include "Game.h"

// Variabili di tipo mesh
Mesh barrels((char *) "../assets/obj/barrels.obj");

Junk::Junk(const Point3 &initialPosition, const float spawnTime, const float fallSpeed, const Vector3 &rotation)

        : initialPosition(initialPosition), spawnTime(spawnTime), fallSpeed(fallSpeed), rotation(rotation) {}

Point3 Junk::getPosition(const float gameSeconds) const {
    const float lifeTime = gameSeconds - spawnTime;
    return initialPosition + -Vector3::GetUpVector() * fallSpeed * lifeTime;
}

void Junk::render(const float gameSeconds, bool useShadow) const {
	const Point3 currentPosition = getPosition(gameSeconds);
	static constexpr float junkScale = 0.1f;
    glPushMatrix();
    {
        glTranslate(currentPosition);
        glScalef(junkScale,junkScale,junkScale);
        glRotatef(rotation.X(),1.f,0.f,0.f); // rotazione intorno all'asse X
        glRotatef(rotation.Y(),0.f,1.f,0.f); // rotazione intorno all'asse Y
        glRotatef(rotation.Z(),0.f,0.f,1.f); // rotazione intorno all'asse Z

		glBindTexture(GL_TEXTURE_2D, (GLuint)Texture::Barrels);
		glEnable(GL_TEXTURE_2D);
        barrels.RenderNxV(/*usTexCoords*/ true);
		glDisable(GL_TEXTURE_2D);
        glColor4f(1.f,1.f,1.f,1.f);
    }
    glPopMatrix();
    if (currentPosition.Y() > 0.f) {
        // Render Shadow
        if (useShadow) {
            glPushMatrix();
            {
                glTranslatef(currentPosition.X(), 0.35f, currentPosition.Z());

                glScalef(junkScale * (1 - (currentPosition.Y() / /* spawnHeight */40.f)),
                         junkScale * (1 - (currentPosition.Y() / /* spawnHeight */40.f)),
                         junkScale * (1 - (currentPosition.Y() / /* spawnHeight */40.f)));
                glScalef(1.01f, 0.f, 1.01f);
                glRotatef(rotation.X(), 1.f, 0.f, 0.f); // rotazione intorno all'asse X
                glRotatef(rotation.Y(), 0.f, 1.f, 0.f); // rotazione intorno all'asse Y
                glRotatef(rotation.Z(), 0.f, 0.f, 1.f); // rotazione intorno all'asse Z
                glColor3f(12.f / 255.f, 12.f / 255.f, 12.f / 255.f); // colore fisso
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                barrels.RenderNxV(/*usTexCoords*/ false);
                glDisable(GL_BLEND);
                glColor4f(1.f, 1.f, 1.f, 1.f);
            }
            glPopMatrix();
        }
    }
    utils::checkGLError(__FILE__, __LINE__);
}
