#pragma once
#include <SFML\Graphics.hpp>
#include "Clickeable.h"
#include "Entity.h"
class GameOver
{
public:
	GameOver();
	~GameOver();
	void render(sf::RenderWindow& window);
	void update(float time);
	void handleInput(sf::RenderWindow& window);
	bool clicked;
private:
	Entity background_;
	Clickeable restart_;
	
};