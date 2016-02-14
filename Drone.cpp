#include "Drone.h"

Drone::Drone(const std::string path, std::initializer_list<sf::Vector2f> points) : Entity(path, sf::Vector2f())
{
	for (sf::Vector2f i : points)
	{
		locations_.push_back(i);
	}
	setPosition(locations_[0]);
}

Drone::~Drone()
{
}
