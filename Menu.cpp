#include "Menu.h"

Menu::Menu() : background_("assets/backgrounds/menubackground.jpg", sf::Vector2f()),
title_("assets/titleicons/title.png", sf::Vector2f(0, 400)), selection_(NONE)
{
	buttons_.push_back(new Clickeable("assets/titleicons/start2.png", "assets/titleicons/start1.png", sf::Vector2f()));
	buttons_.push_back(new Clickeable("assets/titleicons/exit2.png", "assets/titleicons/exit1.png", sf::Vector2f()));
}


Menu::~Menu()
{
}

void Menu::render(sf::RenderWindow & window)
{
	//background_.render(window);
	title_.setPosition(sf::Vector2f(window.getSize().x / 2 - title_.getBounds().width / 2, 200));
	title_.render(window);
	buttons_[START]->setPosition(sf::Vector2f(window.getSize().x / 2 - buttons_[START]->getBounds().width / 2, 400));
	buttons_[EXIT]->setPosition(sf::Vector2f(window.getSize().x / 2 - buttons_[EXIT]->getBounds().width / 2, 500));
	for (Clickeable* i : buttons_)
	{
		i->render(window);
	}
}

void Menu::update(float time)
{
	selection_ = NONE;
	for (Clickeable* i : buttons_)
	{
		i->update(time);
	}
	if (buttons_[EXIT]->clicked_)
		selection_ = EXIT;
	if (buttons_[START]->clicked_)
		selection_ = START;
}

void Menu::handleInput(sf::RenderWindow& window)
{
	for (Clickeable* i : buttons_)
	{
		i->handleInput(window);
	}
}