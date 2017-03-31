// Created by Margherita Donnici on 1/7/17.

#include "MenuManager.h"
#include "Colors.h"
#include "Game.h"
#include "Utils.h"
#include <deque>

void MenuManager::setMenuState(MenuState state) {
    currentMenuState = state;
}

void MenuManager::render(const Game &game) const {
    switch (currentMenuState) {
        case MenuState::gameOver: {
            renderGameOver(game);
            break;
        }
        case MenuState::graphicsMenu: {
            renderGraphicsMenu(game);
            break;
        }
    }
}

bool MenuManager::capturesInput() const {
  return currentMenuState != MenuState::none;
}

void MenuManager::onInputEvent(InputEvent inputEvent, Game& game) {
  if (currentMenuState == MenuState::gameOver) {
    if (inputEvent == InputEvent::QuitMenu) {
      exit(0);
    }
  }
  if (currentMenuState == MenuState::graphicsMenu) {
    if (inputEvent == InputEvent::ToggleHelpMenu) {
      setMenuState(MenuState::none);
      game.setState(GameState::Playing);
    }
  }
}

void MenuManager::renderGameOver(const Game &game) const {
    // settiamo il viewport
    glViewport(0, 0, (GLsizei) game.screenSize.X(), (GLsizei) game.screenSize.Y());


    // colore di sfondo (fuori dal mondo)
    glClearColor(0, 0, 0, 1);

    // riempe tutto lo screen buffer di pixel color sfondo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  utils::setCoordToPixel(game.screenSize);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

    // Preparo stringa punteggio
    std::stringstream scoreStream;
    scoreStream << "Points: " << game.getScore();
    Point2 scoreStringPosition = {(game.screenSize.X() / 2 - 85), (game.screenSize.Y() / 2)};
    std::string scoreString = scoreStream.str();

    std::string gameOverString = "GAME OVER";
    Point2 gameOverStringPosition = {(game.screenSize.X() / 2 - 85), (game.screenSize.Y() / 2 + 100)};

    std::string pressEscString = "Premi ESC per uscire";
    Point2 pressEscStringPosition = {(game.screenSize.X() / 2 - 120), (game.screenSize.Y() / 2 - 100)};

    game.textRenderer.render(Colors::Yellow(), Colors::Black(), scoreString.c_str(), scoreStringPosition, blended, big,
                             false);
    game.textRenderer.render(Colors::Yellow(), Colors::Black(), gameOverString.c_str(), gameOverStringPosition, blended,
                             big, false);
    game.textRenderer.render(Colors::Yellow(), Colors::Black(), pressEscString.c_str(), pressEscStringPosition, blended,
                             small, false);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

  glFinish();
}

void MenuManager::renderGraphicsMenu(const Game &game) const {

    // Setup delle matrici

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    {
        utils::setCoordToPixel(game.screenSize);

        // Disegno il menu

        glPushMatrix();
        {

            /* Converto le coordinate in caso la dimensione della finestra è cambiata */
            const Point2 screenSizeCorrection = game.screenSize / game.defaultScreenSize;
            glScalef(screenSizeCorrection.X(), screenSizeCorrection.Y(), 1.f);


            glPushMatrix();
            {
                // Mi preparo a disegnare lo sfondo del menu
                const Point2 menuPosition = Point2{game.defaultScreenSize.X() - 600, game.defaultScreenSize.Y() - 630};
               const Point2 menuSize = Point2{500.f, 500.f};
                glTranslatef(menuPosition.X(), menuPosition.Y(), 1.f);
                glScalef(menuSize.X(), menuSize.Y(), 1.f);
                glEnable (GL_BLEND);
                glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                {
                    glBegin(GL_QUADS);
                    {
                        glColor4f(0.f, 0.f, 0.f, 0.5f);
                        glVertex2f(0.f, 0.f);
                        glVertex2f(0.f, 1.f);
                        glVertex2f(1.f, 1.f);
                        glVertex2f(1.f, 0.f);
                        glColor4f(1.f, 1.f, 1.f, 1.f);
                    }
                    glEnd();
                }

                glDisable(GL_BLEND);

                glFinish();
            }
            glPopMatrix();

            std::deque<menuElement> menuElementsList;
            for (int i = 0; i < menuStrings.size(); ++i){
                Point2 position = {};
                menuElementsList.emplace_front(
                        menuStrings.at(i),
                        Point2{game.defaultScreenSize.X() - 550,
                         game.defaultScreenSize.Y() - 230 - i*50}
                );
            }

            for (const menuElement& element : menuElementsList){
                game.textRenderer.render(Colors::Yellow(), Colors::Black(),
                                         element.string.c_str(), element.position,
                                         blended, small, /*wrapping*/ false);
            }

        }
        glPopMatrix();

    }
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}


