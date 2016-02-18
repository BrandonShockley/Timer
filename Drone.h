#pragma once
#include <vector>
#include <string>
#include <SFML\Graphics.hpp>
#include "Entity.h"

class Drone : public Entity
{
public:
	Drone(std::vector<sf::Vector2f> points);
	~Drone();
	virtual void update(float time) override;
private:
	std::vector<sf::Vector2f> locations_;
	sf::Vector2f lastLocation_;
	sf::Vector2f nextLocation_;
	unsigned int currentLocation_;

	sf::Vector2f velocity_;
	sf::Vector2f acceleration_;

	float angle_;

	static const std::string DEFAULT_DRONE_TEXTURE;
	static const float MAX_LINEAR_SPEED;
	/*
	Plan:
	Constructed with a list of coordinates that are pushed into a vector of sf::vectors
	On construction, move at certain speed to first coordinate, then second, etc.
	*/
};

