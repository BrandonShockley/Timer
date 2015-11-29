#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include "Entity.h"

enum GameState
{
	RUNNING,
	PAUSED,
	STOPPED
};

class Game
{
public:
	Game(sf::RenderWindow& window);
	~Game();
private:
	void gameLoop();
	void processInput();
	void update();
	void render();

	static const unsigned int TARGET_FRAMERATE;

	sf::RenderWindow& window_;
	GameState gameState_;
	std::vector<Entity*> entities_;
};

