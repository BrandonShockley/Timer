#pragma once
#include <SFML\Graphics.hpp>
#include "Entity.h"

class DrainBar : public Entity
{
public:
	DrainBar();
	~DrainBar();
private:
	sf::RectangleShape bar_;
};