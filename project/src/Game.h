// Created by Margherita Donnici on 1/7/17.

#pragma once

#include "JunkManager.h"
#include "EnemyManager.h"
#include "TextRenderer.h"
#include "Minimap.h"
#include "Submarine.h"
#include "MenuManager.h"
#include "Minimap.h"
#include "Map.h"
#include "Input.h"
#include "GameState.h"
#include "Tutorial.h"

namespace CameraState {
    enum Type {
        CAMERA_BACK_SUB,
        CAMERA_TOP_FIXED,
        CAMERA_LATERAL_SUB,
        CAMERA_TOP_SUB,
        CAMERA_MOUSE,
        CAMERA_MAX
    };
}

class Game {
public:

    Submarine sub;
    JunkManager junkManager;
    EnemyManager enemyManager;
    MenuManager menuManager;
    TextRenderer textRenderer;
    Map map;
    Minimap minimap;
    Input inputManager;
    Tutorial tutorial;

	static Point2 defaultScreenSize;
	Point2 screenSize;

    Game();

    int getScore() const {
        return score;
    }

    CameraState::Type getCameraState() const {
        return cameraState;
    }

    // Angoli che definiscono la vista
    float viewAlpha = 20.f;
    float viewBeta = 40.f;
    float eyeDist = 5.f; // Distanza dell'occhio dall'origine

    void switchCamera();

    void init();

    void doStep();

    GameState getCurrentState() const;

    void render();

    void setState(const GameState newState);

    int incrementScore(int inc);

    void EatKey(int keycode, bool pressed, bool joystick);

    void EatJoyAxis(int axisIndex, int axisValue);

	float getGameSeconds() const {return gameSeconds;}

    bool useShadow = true;

private:
    static constexpr float gameDuration = 90.f; // in seconds
	float previousElapsedSeconds = 0.f;
	float gameSeconds = 0.f;

    void setCamera() const;

    // CAMERA SETTINGS
    CameraState::Type cameraState = CameraState::CAMERA_BACK_SUB;



    int score = 0;
    GameState gameState;

};