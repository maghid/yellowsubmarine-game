// Created by Margherita Donnici on 12/29/16.

#pragma once

#include "point2.h"
#include "point3.h"
#include <vector>

class Game;
class Point3;


struct Rock
{
	Point3 position;
	Vector3 scale;
	Vector3 rotation;

	Rock(const Point3& Position, const Vector3& Scale, const Vector3& Rotation)
		: position{Position},
		  scale{Scale},
		  rotation{Rotation}
	{
	}
};

class Map{
public:
	void initialize();
	void render(const Game& game) const; // disegna i vari elementi della scena
    static constexpr float width = 100.f; // ampiezza della mappa dove si gioca
	static constexpr float terrainHeight = 0.f;
	static constexpr float terrainWidth = 400.f; // ampiezza mappa disegnata
	static constexpr float waterHeight = 40.f;
	static constexpr float waterPanSpeed = 0.05f;
	static constexpr int rockCount = 50;
	bool caustics = true;
    std::vector<Rock> rocks;
private:
    void drawSand(const Point3& subPosition) const;
	void drawWaterBackground() const;
	void drawPlane(const float width, const int quadCount, const float tileSize, const Point3& center = Point3{0.f, 0.f, 0.f}) const;
	void drawCaustics(const Game& game) const;
	void drawRocks() const;


	
};
