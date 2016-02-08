#include "Game.h"
#include "Player.h"

const unsigned int Game::TARGET_FRAMERATE = 60;

Game::Game(sf::RenderWindow & window) : window_(window)
{
	gameState_ = GameState::PLAYING;
	levels_.push_back(new Level("maps/test.tmx"));
	entities_.push_back(new Player(levels_[0]->getPlayerSpawn()));
	gameLoop();
}

Game::~Game()
{
	for (unsigned int i = 0; i < entities_.size(); i++)
	{
		entities_.erase(entities_.begin() + i);
	}
}

void Game::gameLoop()
{
	sf::Clock clock;
	float pastTime = 0.0f;
	while (gameState_ != GameState::STOPPED)
	{
		pastTime = clock.getElapsedTime().asSeconds();
		processInput();
		while (pastTime < 1.0f / TARGET_FRAMERATE) 
		{
			float currentTime = clock.getElapsedTime().asSeconds();
			float deltaTime = currentTime - pastTime;
			pastTime = currentTime;
			update(deltaTime);
		}
		render();
		clock.restart();
		while (!window_.hasFocus())
		{}
	}
}

void Game::processInput()
{
	sf::Event e;
	while (window_.pollEvent(e))
	{
		switch (e.type)
		{
		case sf::Event::Closed:
			gameState_ = GameState::STOPPED;
		}
	}
	for (Level* i : levels_)
	{
		i->handleInput(window_);
	}
}

void Game::update(float time)
{
	for (Level* i : levels_)
	{
		i->update(time);
	}
}

void Game::render()
{
	for (Level* i : levels_)
	{
		i->render(window_);
	}
	window_.display();
	window_.clear();
}
