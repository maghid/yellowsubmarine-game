// Created by Margherita Donnici on 1/7/17.

#include <GL/glu.h>
#include "Game.h"
#include "Utils.h"
#include "Colors.h"

Game::Game() {}

Point2 Game::defaultScreenSize = Point2{750.f, 750.f};

int Game::incrementScore(int inc) {
    return score += inc;
}

void Game::init() {
	textRenderer.initialize();
	map.initialize();
    setState(GameState::Playing);
    screenSize  = defaultScreenSize;
}

void Game::doStep() {
	const float deltaSeconds = utils::get_elapsed_seconds() - previousElapsedSeconds;
	previousElapsedSeconds = utils::get_elapsed_seconds();
	if (gameState == GameState::Playing) {
		gameSeconds += deltaSeconds;
		sub.DoStep(*this);
		junkManager.doStep(*this);
		enemyManager.doStep(*this);
	}
}

GameState Game::getCurrentState() const {
    return gameState;
}


void Game::setState(const GameState newState) {
    if (gameState == newState) return;
    gameState = newState;
    switch (gameState) {
        case GameState::GameOver: {
            menuManager.setMenuState(MenuState::gameOver);
            break;
        }

    }


}

void Game::render() {

            glLineWidth(3.f); // linee larghe
            // settiamo il viewport
            glViewport(0, 0,  (GLsizei)screenSize.X(),  (GLsizei)screenSize.Y());
            // settiamo la matrice di proiezione
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(70, //fovy,
                           screenSize.X() / screenSize.Y(),//aspect Y/X,
                           0.2,//distanza del NEAR CLIPPING PLANE in coordinate vista
                           1000  //distanza del FAR CLIPPING PLANE in coordinate vista
            );

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

			// riempe tutto lo screen buffer di pixel color sfondo
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            setCamera();

			GLfloat lightColor[4] = {9.f / 255.f, 61.f / 255.f, 83.f / 255.f, 1.0f};

			// Luce del sole
			{
				float direction[4] = {0.0f, 1.f, 0.5f, 0.f}; // ultima comp=0 => luce direzionale
				glLightfv(GL_LIGHT0, GL_POSITION, direction);

				float ambient[4] = {0.0f, 0.0f, 0.0f, 1.f};
				glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor );
				float specular[4] = {0.f, 0.f, 0.f, 1.f};
				glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
				glEnable(GL_LIGHT0);
				glEnable(GL_LIGHTING);
			}


            // Enable fog drawing
            // Reference: https://www.opengl.org/sdk/docs/man2/xhtml/glFog.xml

            {
                GLfloat* fogColor = lightColor;
                glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]); // Set the background to the fog color
                glFogi(GL_FOG_MODE, GL_EXP);
                glFogfv(GL_FOG_COLOR, fogColor);

                glFogf(GL_FOG_DENSITY, 0.05f);        // For exponential fog

                glHint(GL_FOG_HINT, GL_NICEST);

                // For linear fog
                glFogf(GL_FOG_START, 10.0f);
                glFogf(GL_FOG_END, 200.0f);

                glEnable(GL_FOG);
            }

            map.render(*this);
            sub.render(*this);
            junkManager.renderJunk(*this);
            enemyManager.renderEnemies(*this);

            // attendiamo la fine della rasterizzazione di
            // tutte le primitive mandate
            glFinish();

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);

            // Passiamo al disegno 2D
            utils::setCoordToPixel(screenSize);

            // Disegno del turbo


            const Point2 turboIndicatorSize{120.f, 35.f};
            const Point2 bottomLeftCorner{screenSize.X() - (turboIndicatorSize.X() + /* offset */ 30 ) , screenSize.Y() - ( turboIndicatorSize.Y() + /* offset */ 30 )};

            glScissor(	(int)bottomLeftCorner.X(),
                               (int)bottomLeftCorner.Y(),
                          (GLsizei) ((int) turboIndicatorSize.X() * sub.getTurboCharge(*this)),
                                (int) turboIndicatorSize.Y());
            if (sub.getTurboCharge(*this) == 0) {
                Point2 textPosition = bottomLeftCorner;
                textPosition.X() = textPosition.X() - 60;
                textRenderer.render(Colors::Red(),Colors::Black(),"TURBO!", textPosition, blended, big, false);
            }
			glFrontFace(GL_CCW);
            glEnable(GL_SCISSOR_TEST);

            {
              glDisable(GL_TEXTURE_2D);

              glBegin(GL_TRIANGLES);
              {
                glColor3f(1.f, 0.f, 0.f);
                glVertex2f(bottomLeftCorner.X(), bottomLeftCorner.Y());
                glColor3f(0.f, 1.f, 0.f);
                glVertex2f(bottomLeftCorner.X() + turboIndicatorSize.X(),
                           bottomLeftCorner.Y() + 0.f);
                glColor3f(0.f, 1.f, 0.f);
                glVertex2f(bottomLeftCorner.X() + turboIndicatorSize.X(),
                           bottomLeftCorner.Y() + turboIndicatorSize.Y());
              }
              glEnd();
              glColor3f(1.f, 1.f, 1.f);
            }

            glDisable(GL_SCISSOR_TEST);
			glFrontFace(GL_CW);

            minimap.render(*this);


    {
        /* Punteggio */
        std::stringstream scoreStream;
        scoreStream << "Points: " << getScore();
        Point2 textPosition = {(float) 30, (screenSize.Y() - 50)};
        std::string scoreString = scoreStream.str();
        textRenderer.render(Colors::Yellow(), Colors::Black(), scoreString.c_str(), textPosition, blended, small,
                            false);
    }
    {
        /* Tempo */
        std::stringstream timeStream;
        std::stringstream timeLeftStream;
        float gameSecondsNow = getGameSeconds();
        int timeLeft = (int)gameDuration - (int)gameSecondsNow + 9; // 9 = duration del tutorial
        if (timeLeft == 0) {
            setState(GameState::GameOver);
        }
        if (timeLeft < 60) {
            if (timeLeft < 10){
                timeLeftStream << "00:0" << timeLeft;
            } else {
                timeLeftStream << "00:" << timeLeft;
            }
        } else {
            if (timeLeft-60 < 10) {
                timeLeftStream << "01:0" << timeLeft - 60;
            } else {
                timeLeftStream << "01:" << timeLeft - 60;
            }
        }

        Point2 textPosition = {(float) 30, (screenSize.Y() - 90)};
        std::string timeString = timeLeftStream.str();
        if (gameSecondsNow >= 9) {
            textRenderer.render(Colors::Yellow(), Colors::Black(), timeString.c_str(), textPosition, blended, small,
                                false);
        }
    }
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);

    switch (gameState) {

        case GameState::GameOver : {
            menuManager.render(*this);
        }

        case GameState::Paused : {
            menuManager.render(*this);
        }

    }
    glColor3f(1.f, 1.f, 1.f);

    tutorial.render(*this);

    glFinish();
    utils::checkGLError(__FILE__, __LINE__);
}

void Game::setCamera() const {

    double px = sub.getPosition().X();
    double py = sub.getPosition().Y();
    double pz = sub.getPosition().Z();
    double angle = sub.getFacing();
    double cosf = cos(angle * M_PI / 180.0);
    double sinf = sin(angle * M_PI / 180.0);
    double camd, camh, ex, ey, ez, cx, cy, cz;
    double cosff, sinff;

// controllo la posizione della camera a seconda dell'opzione selezionata
    switch (cameraState) {
        case CameraState::CAMERA_BACK_SUB:
            camd = 2.5;
            camh = 1.0;
            ex = px + camd * sinf;
            ey = py + camh;
            ez = pz + camd * cosf;
            cx = px - camd * sinf;
            cy = py + camh;
            cz = pz - camd * cosf;
            gluLookAt(ex, ey, ez, cx, cy, cz, 0.0, 1.0, 0.0);
            break;
        case CameraState::CAMERA_LATERAL_SUB:
            camd = 6.0;
            camh = 1.15;
            angle = sub.getFacing() - 40.0;
            cosff = cos(angle*M_PI/180.0);
            sinff = sin(angle*M_PI/180.0);
            ex = px + camd*sinff;
            ey = py + camh;
            ez = pz + camd*cosff;
            cx = px - camd*sinf;
            cy = py + camh;
            cz = pz - camd*cosf;
            gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
            break;
        case CameraState::CAMERA_TOP_FIXED:
            camd = 2.5;
            camh = 1.0;
            ex = px + camd * sinf;
            ey = py + camh;
            ez = pz + camd * cosf;
            cx = px - camd * sinf;
            cy = py + camh;
            cz = pz - camd * cosf;
            gluLookAt(ex, ey + 5, ez, cx, cy, cz, 0.0, 1.0, 0.0);
            break;
        case CameraState::CAMERA_TOP_SUB:
            camd = 0.5;
            camh = 0.55;
            cosff = cos(angle * M_PI / 180.0);
            sinff = sin(angle * M_PI / 180.0);
            ex = px + camd * sinff;
            ey = py + camh;
            ez = pz + camd * cosff;
            cx = px - camd * sinf;
            cy = py + camh;
            cz = pz - camd * cosf;
            gluLookAt(ex, ey, ez, cx, cy, cz, 0.0, 1.0, 0.0);
            break;
        case CameraState::CAMERA_MOUSE:
            glTranslatef(0, 0, -eyeDist);
            glRotatef(viewBeta, 1, 0, 0);
            glRotatef(viewAlpha, 0, 1, 0);
            break;
    }
}

void Game::switchCamera() {
    cameraState = (CameraState::Type) (((int) cameraState + 1) % (int) CameraState::CAMERA_MAX);
}

void Game::EatKey(int keycode, bool pressed, bool joystick) {
    inputManager.EatKey(keycode, pressed, joystick, *this);
}

void Game::EatJoyAxis(int axisIndex, int axisValue) {
    inputManager.EatJoyAxis(axisIndex, axisValue);
}




