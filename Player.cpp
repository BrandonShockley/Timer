#include "Player.h"

const float Player::GRAVITY = 10000;
const float Player::JUMP_VELOCITY = -6000;
const float Player::MOVE_ACCELERATION = 30000;
const float Player::MAX_X_SPEED = 2000;
const float Player::MAX_Y_SPEED = 4000;
const float Player::X_DRAG = 10000;
const std::string Player::DEFAULT_PLAYER_TEXTURE = "assets/PlayerAnimation/idle.png";
const std::string Player::DEFAULT_ANIMATION_PATH = "assets/PlayerAnimation";
const unsigned int Player::DEFAULT_ANIMATION_FRAMES = 60;

//Path is to folder containing animation images
Player::Player(sf::Vector2f position) : 
	Entity(DEFAULT_PLAYER_TEXTURE, position),
	jumped_(false), 
	idleAnimation_(Animation(DEFAULT_ANIMATION_PATH + "/idle.png", 1)),
	runAnimation_(Animation(DEFAULT_ANIMATION_PATH + "/running.png", DEFAULT_ANIMATION_FRAMES))
{

}

Player::~Player()
{
}

void Player::update(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	
	handlePhysics(time, grid, tileBounds);
}

void Player::handleInput(sf::RenderWindow & window)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround_.colliding)
	{
		jumped_ = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		state_ = State::MOVING_RIGHT;
		return;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		state_ = State::MOVING_LEFT;
		return;
	}
	state_ = State::IDLE;
}

void Player::render(sf::RenderWindow & window)
{
	switch (state_)
	{
	case State::IDLE:
		sprite_.setTexture(idleAnimation_.getNextFrame());
		break;
	case State::MOVING_LEFT:
		sprite_.setTexture(runAnimation_.getNextFrame());
		//Flips the texture over
		sprite_.setTextureRect(sf::IntRect(sprite_.getTexture()->getSize().x, 0, -(int)sprite_.getTexture()->getSize().x, sprite_.getTexture()->getSize().y));
		break;
	case State::MOVING_RIGHT:
		sprite_.setTexture(runAnimation_.getNextFrame());
		//Anti-flip
		sprite_.setTextureRect(sf::IntRect(0, 0, sprite_.getTexture()->getSize().x, sprite_.getTexture()->getSize().y));
		break;
	}
	sprite_.setPosition(position_);
	window.draw(sprite_);
}

void Player::handleCollision(std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	//Floor collision
	for (int i = 1; i < sprite_.getGlobalBounds().width; i++)
	{
		if (grid[(position_.x + i) / tileBounds.x][(position_.y + sprite_.getGlobalBounds().height) / tileBounds.y].type == TileType::SOLID)
		{
			onGround_.colliding = true;
			onGround_.displacement = abs(grid[(position_.x + i) / tileBounds.x][(position_.y + sprite_.getGlobalBounds().height) / tileBounds.y].entity->getPosition().y - sprite_.getGlobalBounds().height);
			
			if (velocity_.y > 0)
			{
				velocity_.y = 0;
				position_.y = onGround_.displacement;
				jumped_ = false;
			}
			else if (jumped_)
			{
				velocity_.y = JUMP_VELOCITY;
			}
			break;
		}
		else
			onGround_.colliding = false;
	}
	//Right wall collision
	for (int i = 0; i < sprite_.getGlobalBounds().height; i++)
	{
		if (grid[(position_.x + sprite_.getGlobalBounds().width) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
		{
			collideRight_.colliding = true;
			collideRight_.displacement = abs(grid[(position_.x + sprite_.getGlobalBounds().width) / tileBounds.x][(position_.y + i) / tileBounds.y].entity->getPosition().x - sprite_.getGlobalBounds().width);
			if (velocity_.x > 0)
			{
				acceleration_.x = 0;
				velocity_.x = 0;
				position_.x = collideRight_.displacement;
			}
			break;
		}	
		else
			collideRight_.colliding = false;
	}
	//Left wall collision
	for (int i = 0; i < sprite_.getGlobalBounds().height; i++)
	{
		if (grid[(position_.x) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
		{
			collideLeft_.colliding = true;
			collideLeft_.displacement = abs(grid[(position_.x) / tileBounds.x][(position_.y + i) / tileBounds.y].entity->getPosition().x + tileBounds.x);
			if (velocity_.x < 0)
			{
				acceleration_.x = 0;
				velocity_.x = 0;
				position_.x = collideLeft_.displacement;
			}
			break;
		}
		else
			collideLeft_.colliding = false;
	}
	//Top platform collision //TODO: Fix weird top glitch when not tired
	for (int i = 0; i < sprite_.getGlobalBounds().width; i++)
	{
		if (grid[(position_.x + i) / tileBounds.x][(position_.y) / tileBounds.y].type == TileType::SOLID)
		{
			collideTop_.colliding = true;
			collideTop_.displacement = abs(grid[(position_.x + i) / tileBounds.x][(position_.y) / tileBounds.y].entity->getPosition().y + tileBounds.y);
			if (velocity_.y < 0)
			{
				acceleration_.y = 0;
				velocity_.y = 0;
				position_.y = collideTop_.displacement;
			}
			break;
		}
		else
			collideTop_.colliding = false;
	}
}

void Player::handlePhysics(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	switch (state_)
	{
	case State::IDLE:
		acceleration_ = sf::Vector2f(-(abs(velocity_.x) / velocity_.x)*X_DRAG, 0);
		if (-(abs(velocity_.x) / velocity_.x) != -(abs(velocity_.x + acceleration_.x * time) / (velocity_.x + acceleration_.x * time)))
		{
			acceleration_.x = 0.0f;
			velocity_.x = 0.0f;
		}
		break;
	case State::MOVING_RIGHT:
		acceleration_ = sf::Vector2f(MOVE_ACCELERATION, 0);
		break;
	case State::MOVING_LEFT:
		acceleration_ = sf::Vector2f(-MOVE_ACCELERATION, 0);
		break;
	}

	handleCollision(grid, tileBounds);

	if (!onGround_.colliding)
	{
		acceleration_ += sf::Vector2f(0, GRAVITY);
	}

	if (velocity_.x > MAX_X_SPEED)
		velocity_.x = MAX_X_SPEED;
	else if (velocity_.x < -MAX_X_SPEED)
		velocity_.x = -MAX_X_SPEED;
	if (velocity_.y > MAX_Y_SPEED)
		velocity_.y = MAX_Y_SPEED;
	else if (velocity_.y < -MAX_Y_SPEED)
		velocity_.y = -MAX_Y_SPEED;

	velocity_ += acceleration_ * time;
	position_ += velocity_ * time;
}
