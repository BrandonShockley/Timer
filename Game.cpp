#include "Game.h"

const unsigned int Game::TARGET_FRAMERATE = 60;

Game::Game(sf::RenderWindow & window) : window_(window)
{
	gameState_ = GameState::RUNNING;
	gameLoop();
}

Game::~Game()
{
	for (unsigned int i = 0; i < entities_.size(); i++)
	{
		entities_.erase(entities_.begin() + i);
		delete entities_[i];
	}
}

void Game::gameLoop()
{
	sf::Clock clock;
	while (gameState_ == GameState::RUNNING)
	{
		processInput();
		update();	
		while (clock.getElapsedTime().asSeconds() < 1 / TARGET_FRAMERATE) {}
		render();
	}
}

void Game::processInput()
{
	sf::Event e;
	while (window_.pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
		{
			gameState_ = GameState::STOPPED;
		}
	}
}

void Game::update()
{
}

void Game::render()
{
}
