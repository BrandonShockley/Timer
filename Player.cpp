#include "Player.h"

const float Player::GRAVITY = 3000;
const float Player::JUMP_VELOCITY = -3000;
const std::string Player::DEFAULT_PLAYER_TEXTURE = "assets/player.png";

Player::Player(sf::Vector2f position, const std::string path) : Entity(path, position), jumped_(false)
{

}
Player::~Player()
{
}

void Player::update(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	handleCollision(grid, tileBounds);
	handlePhysics(time);
}

void Player::handleInput(sf::RenderWindow & window)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround_.colliding)
	{
		jumped_ = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		state = State::MOVING_RIGHT;
		return;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		state = State::MOVING_LEFT;
		return;
	}
	state = State::IDLE;
}

void Player::handleCollision(std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	//Gotta change this to check every point along perimeter
	for (int i = 0; i < sprite_.getGlobalBounds().width; i++)
	{
		if (grid[(position_.x + i) / tileBounds.x][(position_.y + sprite_.getGlobalBounds().height) / tileBounds.y].type == TileType::SOLID)
		{
			onGround_.colliding = true;
			onGround_.displacement = abs(grid[(position_.x + i) / tileBounds.x][(position_.y + sprite_.getGlobalBounds().height) / tileBounds.y].entity->getPosition().y - sprite_.getGlobalBounds().height);
			break;
		}
		else
			onGround_.colliding = false;
	}
	for (int i = 0; i < sprite_.getGlobalBounds().height; i++)
	{
		if (grid[(position_.x + sprite_.getGlobalBounds().width) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
		{
			collideRight_.colliding = true;
			break;
		}	
		else
			collideRight_.colliding = false;
	}
	for (int i = 0; i < sprite_.getGlobalBounds().height; i++)
	{
		if (grid[(position_.x) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
		{
			collideLeft_.colliding = true;
			break;
		}
		else
			collideLeft_.colliding = false;
	}
	for (int i = 0; i < sprite_.getGlobalBounds().width; i++)
	{
		if (grid[(position_.x + i) / tileBounds.x][(position_.y) / tileBounds.y].type == TileType::SOLID)
		{
			collideTop_.colliding = true;
			break;
		}
		else
			collideTop_.colliding = false;
	}
}

void Player::handlePhysics(float time)
{
	switch (state)
	{
	case State::IDLE:
		acceleration_ = sf::Vector2f(0, 0);
		break;
	case State::MOVING_RIGHT:
		acceleration_ = sf::Vector2f(2000, 0);
		break;
	case State::MOVING_LEFT:
		acceleration_ = sf::Vector2f(-2000, 0);
		break;
	}
	if (!onGround_.colliding)
	{
		acceleration_ += sf::Vector2f(0, GRAVITY);
	}
	else if (velocity_.y > 0)
	{
		velocity_.y = 0;
		position_.y = onGround_.displacement;
		jumped_ = false;
	}
	else if (jumped_)
	{
		velocity_.y = JUMP_VELOCITY;
	}
	
	if (collideRight_.colliding && velocity_.x > 0)
	{
		velocity_.x = 0;
	}
	if (collideLeft_.colliding && velocity_.x < 0)
	{
		velocity_.x = 0;
	}
	if (collideTop_.colliding && velocity_.y < 0)
	{
		velocity_.y = 0;
	}
	velocity_ += acceleration_ * time;
	position_ += velocity_ * time;
}
