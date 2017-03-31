#include "Textures.h"
#include "Submarine.h"
#include "JunkManager.h"
#include "EnemyManager.h"
#include "Physics.h"
#include "Minimap.h"
#include "Utils.h"
#include "MenuManager.h"
#include "Game.h"

Uint32 nstep = 0; // numero di passi di FISICA fatti fin'ora

// Frames Per Seconds

float fps = 0; // valore di fps dell'intervallo precedente
int fpsNow = 0; // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
Uint32 timeLastInterval = 0; // quando e' cominciato l'ultimo intervallo

static Game game;

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win) {

    // un frame in piu'!!!
    fpsNow++;
    game.render();
    // ho finito: buffer di lavoro diventa visibile
    glFinish();
    utils::checkGLError(__FILE__, __LINE__);
    SDL_GL_SwapWindow(win);
}

void redraw() {
    // ci automandiamo un messaggio che (s.o. permettendo)
    // ci fara' ridisegnare la finestra
    SDL_Event e;
    e.type = SDL_WINDOWEVENT;
    e.window.event = SDL_WINDOWEVENT_EXPOSED;
    SDL_PushEvent(&e);
}

int main(int argc, char *argv[]) {

    SDL_Window *win;
    SDL_GLContext mainContext;
    Uint32 windowID;
    SDL_Joystick *joystick;

    // Inizializzazione di SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    // Inizializzazione di SDL_TTF
    if (TTF_Init() < 0) {
        fprintf(stderr, "Impossibile inizializzare TTF: %s\n", SDL_GetError());
        SDL_Quit();
        return (2);
    }


    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);

    // Imposto i parametri della finestra OpenGL
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Creo una finestra
    win = SDL_CreateWindow(argv[0], 0, 0, (int) Game::defaultScreenSize.X(), (int) Game::defaultScreenSize.Y(),
                           SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    //Create our OpengGL context and attach it to our window
    mainContext = SDL_GL_CreateContext(win);

    // Smoothing modes (
    {
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    // Luce
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE); // opengl, per favore, rinormalizza le normali
    // prima di usarle
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW); // consideriamo Front Facing le facce ClockWise
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_POLYGON_OFFSET_FILL); // caro openGL sposta i
    // frammenti generati dalla
    // rasterizzazione poligoni
    glPolygonOffset(1.f, 1.f);             // indietro di 1

    utils::checkGLError(__FILE__, __LINE__);

    if (!Texture::LoadAllTextures()) return -1;

    game.init();


    bool done = false;
    while (!done) {

        SDL_Event e;

        // guardo se c'e' un evento:
        if (SDL_PollEvent(&e)) {
            // se si: processa evento
            switch (e.type) {
                case SDL_KEYDOWN:
                    game.EatKey(e.key.keysym.sym, true, false);
                    break;
                case SDL_KEYUP:
                    game.EatKey(e.key.keysym.sym, false, false);
                    break;
                case SDL_QUIT:
                    done = true;
                    break;
                case SDL_WINDOWEVENT:
                    // dobbiamo ridisegnare la finestra
                    if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
                        rendering(win);
                    else {
                        windowID = SDL_GetWindowID(win);
                        if (e.window.windowID == windowID &&
                            e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                            game.screenSize = Point2{(float) e.window.data1, (float) e.window.data2};
                            rendering(win);
                            redraw(); // richiedi ridisegno
                            break;
                        }
                    }
                    break;

                case SDL_MOUSEMOTION:
                    if ((e.motion.state & SDL_BUTTON(1)) && game.getCameraState() == CameraState::CAMERA_MOUSE) {
                        game.viewAlpha += e.motion.xrel;
                        game.viewBeta += e.motion.yrel;
                        if (game.viewBeta < +5) game.viewBeta = +5; //per non andare sotto la macchina
                        if (game.viewBeta > +90) game.viewBeta = +90;
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    if (game.getCameraState() == CameraState::CAMERA_MOUSE) {
                        if (e.wheel.y > 0) {
                            // avvicino il punto di vista (zoom in)
                            game.eyeDist = game.eyeDist * 0.9f;
                            if (game.eyeDist < 3) game.eyeDist = 3;
                        };
                        if (e.wheel.y < 0) {
                            // allontano il punto di vista (zoom out)
                            game.eyeDist = game.eyeDist / 0.9f;
                            if(game.eyeDist > 100) game.eyeDist = 100; // impedisco all'utente di uscire dal mondo con la visuale
                        };
                    }
                    break;

                case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
                    game.EatJoyAxis(e.jaxis.axis, e.jaxis.value);
                    break;

                    /* Handle Joystick Button Presses */
                case SDL_JOYBUTTONDOWN:
                    game.EatKey(e.jbutton.button, true, /* joystick */ true);
                    break;

                case SDL_JOYBUTTONUP:
                    game.EatKey(e.jbutton.button, false, /* joystick */ true);
                    break;

            }

        } else {
            // nessun evento: siamo IDLE

            Uint32 timeNow = SDL_GetTicks(); // che ore sono?

            if (timeLastInterval + Physics::fpsSampling < timeNow) {
                fps = 1000.f * ((float) fpsNow) / (timeNow - timeLastInterval);
                fpsNow = 0;
                timeLastInterval = timeNow;
            }

            bool doneSomething = false;
            int guardia = 0; // sicurezza da loop infinito

            // finche' il tempo simulato e' rimasto indietro rispetto
            // al tempo reale...
            while (nstep * Physics::stepMs < timeNow) {
                game.doStep();
                doneSomething = true;
                nstep++;
                timeNow = SDL_GetTicks();
                if (guardia++ > 1000) {
                    done = true;
                    break;
                } // siamo troppo lenti!
            }

            if (doneSomething)
                rendering(win);
                //redraw();
            else {
                // tempo libero!!!
            }
        }

        utils::checkGLError(__FILE__, __LINE__);
    }
    SDL_GL_DeleteContext(mainContext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return (0);
}

