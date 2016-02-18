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
private:
	Entity background_;
};