#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include "Entity.h"
#include "Level.h"
#include "Menu.h"

enum GameState
{
	MENU,
	PLAYING,
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
	void update(float time);
	void render();

	static const unsigned int TARGET_FRAMERATE;

	unsigned int currentLevel_;

	sf::RenderWindow& window_;
	GameState gameState_;
	std::vector<Entity*> entities_;
	std::vector<Level*> levels_;
	Menu menu_;
};

