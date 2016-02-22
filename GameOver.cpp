#include "GameOver.h"



GameOver::GameOver() : background_("assets/backgrounds/youtried.png", sf::Vector2f(100, 100)), 
restart_("assets/titleicons/restart1.png", "assets/titleicons/restart2.png", sf::Vector2f(0, 0)),
clicked(false)
{
}


GameOver::~GameOver()
{
}

void GameOver::render(sf::RenderWindow& window)
{
	background_.setPosition(sf::Vector2f(window.getSize().x / 2 - background_.getBounds().width / 2, 200));
	background_.render(window);
	restart_.setPosition(sf::Vector2f(window.getSize().x / 2 - restart_.getBounds().width / 2, 500));
	restart_.render(window);
}

void GameOver::update(float time)
{
	background_.update(time);
	restart_.update(time);
	if (restart_.clicked_)
		clicked = true;
	else
		clicked = false;
}

void GameOver::handleInput(sf::RenderWindow & window)
{
	restart_.handleInput(window);
}
