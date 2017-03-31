#pragma once


#include "point3.h"
#include "Map.h"
#include "InputEvent.h"

class Game;
class Input;
class Enemy;
enum class InputEvent;


class Submarine {

public:
    // Metodi
    void Init(); // inizializza variabili
    void render(const Game &game) const; // disegna a schermo
    void DoStep(Game &game); // computa un passo del motore fisico
    Submarine() { Init(); } // costruttore

    void onInputEvent(InputEvent inputEvent, Game& game); // gestisce gli eventi di input

    float getRadius() const {
        return 3.f;
    }

    Point3 getPosition() const {
        return {px, py, pz};
    }

	float getFacing() const { return facing;}

    float getTurboCharge(const Game& game)const;

private:
	// STATO DEL SOTTOMARINO
	// (DoStep fa evolvere queste variabili nel tempo)
	float px = 0.f;

	float py, pz, facing; // posizione e orientamento
	float mozzoA, mozzoP, sterzo; // stato interno
	float vx, vy, vz; // velocita' attuale

	// STATS DEL SOTTOMARINO
	// (di solito rimangono costanti)
	float velSterzo, velRitornoSterzo, accMax,
		raggioRuotaA, raggioRuotaP, grip,
		attritoX, attritoY, attritoZ; // attriti

    bool useHeadlight = true;

private:

    //Variabili per il turbo
    bool turbo = false; // TRUE se il turbo è attivo, FALSE altrimenti
    float startTurboTime = 0.f; // (in secondi) ultima data di inizio del turbo
    float nextTurbo = 0.f; // (in secondi) prossima data dopo la quale sarà possibile usare il turbo
    float turboDelay = 30.f; // (in secondi) il turbo può essere usato solo una volta ogni 30 secondi
    float turboDuration = 5.f; // durata del turbo (in secondi)
    float turboAcc = 2.f; // accelerazione del turbo

    // Variabili per evitare che il sottomarino esca dai limiti della mappa "giocabile"
    float maxX = 50.f;
    float minX = -50.f;
    float maxZ = 50.f;
    float minZ = -50.f;
    float maxY = Map::waterHeight;
    float minY = 1.5f;

    void RenderAllParts(bool usecolor) const;
    void SetLight() const;
};
