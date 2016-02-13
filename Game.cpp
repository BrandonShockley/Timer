#include "Game.h"
#include "Player.h"

const unsigned int Game::TARGET_FRAMERATE = 60;

Game::Game(sf::RenderWindow & window) : window_(window), currentLevel_(0)
{
	gameState_ = GameState::PLAYING;
	levels_.push_back(new Level("maps/test.tmx"));
	levels_.push_back(new Level("maps/ping.tmx"));
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
	bool focus = false;
	sf::Clock clock;
	float pastTime = 0.0f;
	while (gameState_ != GameState::STOPPED)
	{
		if (window_.hasFocus())
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
		}
		else
			processInput();
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
	if (gameState_ == GameState::MENU)
	{
		menu_.handleInput(window_);
	}
	if (gameState_ == GameState::PLAYING)
	{
		levels_[currentLevel_]->handleInput(window_);
	}
}

void Game::update(float time)
{
	if (gameState_ == GameState::MENU)
	{
		menu_.update(time);
		if (menu_.selection_ == EXIT)
			gameState_ = GameState::STOPPED;
		if (menu_.selection_ == START)
			gameState_ = GameState::PLAYING;
	}
	if (gameState_ == GameState::PLAYING)
	{
		levels_[currentLevel_]->update(time);
		if (levels_[currentLevel_]->isCompleted() && currentLevel_ != levels_.size())
			currentLevel_++;
		if (currentLevel_ == levels_.size())
			gameState_ = GameState::STOPPED;
	}
}

void Game::render()
{
	if (gameState_ == GameState::MENU)
	{
		menu_.render(window_);
	}
	if (gameState_ == GameState::PLAYING)
	{
		levels_[currentLevel_]->render(window_);
		window_.setMouseCursorVisible(false);
	}
	window_.display();
	window_.clear();
}
