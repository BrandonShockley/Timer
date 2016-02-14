#pragma once
#include <vector>
#include <SFML\Graphics.hpp>
#include "Entity.h"

class Drone : public Entity
{
public:
	Drone(const std::string path, std::initializer_list<sf::Vector2f> points);
	~Drone();
private:
	std::vector<sf::Vector2f> locations_;
	/*
	Plan:
	Constructed with a list of coordinates that are pushed into a vector of sf::vectors
	On construction, move at certain speed to first coordinate, then second, etc.
	*/
};

