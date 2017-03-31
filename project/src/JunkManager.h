// Created by Margherita Donnici on 12/27/16.

#pragma once

#include "Junk.h"
#include <vector>

class Game;
class Submarine;
class Junk;

class JunkManager {
public:
    void renderJunk(const Game& game) const;
    void doStep(Game &game);
	std::vector<Point3> getJunkPositions(const Game& game) const;
private:
    std::vector<Junk> allJunk{};
    static constexpr float spawnHeight = 40.f;
    float nextSpawnDate = 6.f;
	const float spawnInterval = 3.f;

    void handleJunkSpawning(const Game& game);
    void handleCollisions(Game &game);
    bool doesCollide(const Junk& pieceOfJunk, const Game& game) const;
    void spawnPieceOfJunk(const Game& game); };

