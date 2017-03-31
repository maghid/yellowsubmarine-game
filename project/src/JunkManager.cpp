// Created by Margherita Donnici on 12/27/16.

#include "JunkManager.h"
#include "Utils.h"
#include "Map.h"
#include "Game.h"
#include <algorithm>

class Game;

/* Disegna tutti i pezzi di junk sullo schermo */
void JunkManager::renderJunk(const Game& game) const {
    for (const Junk &pieceOfJunk : allJunk) {
        pieceOfJunk.render(game.getGameSeconds(), game.useShadow);
    }
	utils::checkGLError(__FILE__, __LINE__);
}

/* TRUE se c'è una collisione tra i 2 oggetti */
bool JunkManager::doesCollide(const Junk &pieceOfJunk, const Game& game) const {
    const Point3 junkPosition = pieceOfJunk.getPosition(game.getGameSeconds());
    const Point3 submarinePosition = game.sub.getPosition();
    return utils::distance(junkPosition, submarinePosition) < pieceOfJunk.getRadius() + game.sub.getRadius();
}

void JunkManager::handleCollisions(Game &game) {
    const float secondsNow = game.getGameSeconds();

    // Lambda che restituisce TRUE se la y del pieceOfJunk è minore del terrainHeight */
    const auto collidesWithTerrain = [&](const Junk &pieceOfJunk) {
        return pieceOfJunk.getPosition(secondsNow).Y() <
               Map::terrainHeight - pieceOfJunk.getRadius();

    };

    /* Lambda perchè erase prende in input una funzione con un solo argomento Junk */
    /* Restituisce TRUE se oggetto ha una collisione con il sottomarino o il terreno */
    const auto isJunkDestroyed = [&](const Junk &pieceOfJunk) {
        return doesCollide(pieceOfJunk, game) || collidesWithTerrain(pieceOfJunk);
    };

    /* Incrementa punteggio se necessario */
    for (const Junk& pieceOfJunk : allJunk){
        if (doesCollide(pieceOfJunk, game)) {
            game.incrementScore(10);
        }
    }

    /* Rimuove dal vettore tutti i junk che hanno una collisione con il sottomarino */
    allJunk.erase(
            std::remove_if(
                    allJunk.begin(),
                    allJunk.end(),
                    isJunkDestroyed),
            allJunk.end());

}

void JunkManager::handleJunkSpawning(const Game& game) {
    float secondsNow = game.getGameSeconds();
    if (secondsNow > nextSpawnDate) {
        spawnPieceOfJunk(game);
        nextSpawnDate += spawnInterval;
    }
}

void JunkManager::doStep(Game &game) {

    handleJunkSpawning(game);
    handleCollisions(game);

}

std::vector<Point3> JunkManager::getJunkPositions(const Game& game) const
{
	std::vector<Point3> junkPositions{};
	const float elapsedSeconds = game.getGameSeconds();
	for (const Junk& pieceOfJunk : allJunk)
	{
		junkPositions.push_back(pieceOfJunk.getPosition(elapsedSeconds));
	}
	return junkPositions;
}

void JunkManager::spawnPieceOfJunk(const Game& game) {
    const Point3 junkRandomPosition = Point3{utils::random_float() * Map::width - Map::width / 2.f,
                                             spawnHeight,
                                             utils::random_float() * Map::width - Map::width / 2.f
    };
    const Vector3 junkRandomRotation = Vector3{utils::random_float()*360.f, // angolo random tra 0 e 360
                                               utils::random_float()*360.f,
                                               utils::random_float()*360.f
    };
    // Aggiungiamo i detriti al vettore, costruendoli in-place
    allJunk.emplace_back(
            junkRandomPosition,
            /*spawnTime in seconds*/game.getGameSeconds(),
            /*fallSpeed*/3.f,
            junkRandomRotation);
}

