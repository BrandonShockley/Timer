#include "GameOver.h"



GameOver::GameOver() : background_("assets/backgrounds/youtried.png", sf::Vector2f(100, 100))
{
}


GameOver::~GameOver()
{
}

void GameOver::render(sf::RenderWindow& window)
{
	background_.render(window);
}

void GameOver::update(float time)
{
	background_.update(time);
}

void GameOver::handleInput(sf::RenderWindow & window)
{
}
