#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include "Entity.h"
#include "Level.h"
#include "Menu.h"
#include "GameOver.h"

enum GameState
{
	MENU,
	PLAYING,
	GAME_OVER,
	COMPLETE,
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
	sf::View view_;
	GameState gameState_;
	std::vector<Level*> levels_;
	Menu menu_;
	GameOver gameOverScreen_;
};

