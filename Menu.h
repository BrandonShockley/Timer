#pragma once
#include "Entity.h"
#include "Clickeable.h"
#include <vector>
#include <SFML\Graphics.hpp>

enum Selections
{
	START,
	EXIT,
	NONE
};

class Menu
{
public:
	Menu();
	~Menu();
	void render(sf::RenderWindow& window);
	void update(float time);
	void handleInput(sf::RenderWindow& window);
	
	Selections selection_;
private:
	Entity background_;
	Entity title_;
	std::vector<Clickeable*> buttons_;
};

