#include "Winner.h"



Winner::Winner() : background_("assets/backgrounds/win.png", sf::Vector2f(0, 0))
{
}


Winner::~Winner()
{
}

void Winner::render(sf::RenderWindow & window)
{
	background_.setPosition(sf::Vector2f(window.getSize().x / 2 - background_.getBounds().width / 2, 200));
	background_.render(window);
}

void Winner::update(float time)
{
}

void Winner::handleInput(sf::RenderWindow & window)
{
}
