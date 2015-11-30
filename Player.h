#pragma once
#include "Entity.h"
#include <SFML\Graphics.hpp>

enum State
{
	IDLE,
	MOVING_LEFT,
	MOVING_RIGHT,
	JUMPING
};

class Player : public Entity
{
public:
	Player(const std::string path, sf::Vector2f position, sf::RenderWindow& window);
	~Player();

	using Entity::update;
	virtual void update(float time);
	using Entity::handleInput;
	virtual void handleInput(sf::RenderWindow& window);
private:
	void handlePhysics(float time);

	sf::Vector2f velocity_;
	sf::Vector2f acceleration_;
	State state;
};

