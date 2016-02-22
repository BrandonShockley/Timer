#pragma once
#include "Entity.h"
class Winner
{
public:
	Winner();
	~Winner();
	void render(sf::RenderWindow& window);
	void update(float time);
	void handleInput(sf::RenderWindow& window);
private:
	Entity background_;
};

