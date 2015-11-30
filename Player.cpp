#include "Player.h"



Player::Player(const std::string path, sf::Vector2f position, sf::RenderWindow& window) : Entity(path, position)
{

}


Player::~Player()
{
}

void Player::update(float time)
{

	handlePhysics(time);
}

void Player::handleInput(sf::RenderWindow & window)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		state = State::MOVING_RIGHT;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		state = State::MOVING_LEFT;
	}
	state = State::IDLE;
}

void Player::handlePhysics(float time)
{
	switch (state)
	{
	case State::IDLE:
		acceleration_ = sf::Vector2f(0, 0);
		break;
	case State::MOVING_RIGHT:
		acceleration_ = sf::Vector2f(1000, 0);
		break;
	case State::MOVING_LEFT:
		acceleration_ = sf::Vector2f(-1000, 0);
		break;
	}
	velocity_ += acceleration_ * time;
	position_ += velocity_ * time;
}
