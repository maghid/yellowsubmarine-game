// Created by Margherita Donnici on 12/31/16.
#include "Minimap.h"

#include "EnemyManager.h"
#include "Utils.h"
#include "Game.h"

void Minimap::render(
	const Game& game) const {
  const Point2 BottomLeftCorner{game.screenSize.X() - offsetFromRight -
                                    minimapSize.X(),
                                offsetFromBottom};

  // Disegnamo solo sulla minimappa
  glScissor((int)BottomLeftCorner.X(), (int)BottomLeftCorner.Y(),
            (int)minimapSize.X(), (int)minimapSize.Y());
  glEnable(GL_SCISSOR_TEST);

  // Disegno un quadrato su cui verrà applicata la texture del sonar
  glPushMatrix();
  {

    glTranslatef(BottomLeftCorner.X(), BottomLeftCorner.Y(), 1.f);
    glScalef(minimapSize.X(), minimapSize.Y(), 1.f);

    // Draw minimap background
    {
      glBindTexture(GL_TEXTURE_2D, Texture::Sonar);
      glEnable(GL_TEXTURE_2D);

      glBegin(GL_QUADS);
      {
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(0.f, 0.f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(0.f, 1.f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.f, 1.f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.f, 0.f);
      }
      glEnd();

      glDisable(GL_TEXTURE_2D);
    }

    glPointSize(sonarPointSize);
    glPushMatrix();
    {

      glScalef(1.f / Map::width, 1.f / Map::width, 1.f);
      glTranslatef(Map::width / 2.f, Map::width / 2.f, 0.f);

      if (rotatesWithPlayer) {
        glRotatef(game.sub.getFacing() + 180.f, 0.f, 0.f, 1.f);
      }

      if (centersOnPlayer) {
        glTranslatef(-game.sub.getPosition().X(), -game.sub.getPosition().Z(), 0.f);

        // Confini del livello
        glPushMatrix();
        {
          glLineWidth(mapBoundsLineWidth);
          glColor3fv(mapBoundsColor);
          glScalef(-Map::width / 2.f, -Map::width / 2.f, 1.f);
          glBegin(GL_LINE_LOOP);
          {
            glVertex2f(-1.f, -1.f);
            glVertex2f(1.f, -1.f);
            glVertex2f(1.f, 1.f);
            glVertex2f(-1.f, 1.f);
          }
          glEnd();
        }
        glPopMatrix();
      }

      // Draw minimap dots
      glBegin(GL_POINTS);
      {
        // Draw player
        glColor3fv(playercolor);
        glVertex2f(game.sub.getPosition().X(), game.sub.getPosition().Z());

        // Draw enemies
        glColor3fv(enemyColor);
        for (const Point3 &enemyPosition : game.enemyManager.getEnemyPositions()) {
          glVertex2f(enemyPosition.X(), enemyPosition.Z());
        }

        // Draw junk
        glColor3fv(junkColor);
        for (const Point3 &junkPosition : game.junkManager.getJunkPositions(game)) {
          glVertex2f(junkPosition.X(), junkPosition.Z());
        }
      }
      glEnd();

      // Linea sonar
      glPushMatrix();
      {
        glLineWidth(sonarLineWidth);
        const float elapsedSeconds = game.getGameSeconds();
        const Point3 subPosition = game.sub.getPosition();
        glTranslatef(subPosition.X(), subPosition.Z(), 0.f);
        glRotatef(elapsedSeconds * sonarRotationSpeed - game.sub.getFacing(), 0.f, 0.f,
                  1.f);
        glBegin(GL_LINES);
        {
          glColor3fv(sonarLineColor);
          glVertex2f(0.f, 0.f);
          glVertex2f(sonarLineRadius, 0.f);
        }
        glEnd();
      }
      glPopMatrix();

      // Cerchi sonar
      glPushMatrix();
      {
        glLineWidth(sonarCircleLineWidth);
        glColor3fv(sonarCircleColor);
        const Point3 subPosition = game.sub.getPosition();
        glTranslatef(subPosition.X(), subPosition.Z(), 0.f);
        for (int circleIndex = 0; circleIndex < sonarCircleCount;
             circleIndex++) {
          const float circleRadius = circleIndex * sonarCircleRadiusIncrement;
          glBegin(GL_LINE_LOOP);
          {
            const float angleIncrement =
                2.f * (float)M_PI / (float)sonarCircleLineSections;
            for (float angleRadians = 0.f; angleRadians < 2.f * M_PI;
                 angleRadians += angleIncrement) {
              // Equazione di un cerchio: x = r*sin(angolo), y = r*cos(angolo)
              // dobbiamo fare i calcoli noi perche glTranslate e glScale non
              // sono permessi dentro glBegin/glEnd
              glVertex2f(circleRadius * sinf(angleRadians),
                         circleRadius * cosf(angleRadians));
            }
          }
          glEnd();
        }
      }
      glPopMatrix();
    }
    glPopMatrix();
  }
  glPopMatrix();
  glColor3f(1.f,1.f,1.f);

  glDisable(GL_SCISSOR_TEST);

  utils::checkGLError(__FILE__, __LINE__);
}
