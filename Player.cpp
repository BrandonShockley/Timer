#include "Player.h"

const float Player::GRAVITY = 10000;
const float Player::JUMP_VELOCITY = -3300;
const float Player::MOVE_ACCELERATION = 30000;
const float Player::MAX_X_SPEED = 2000;
const float Player::MAX_Y_SPEED = 4000;
const float Player::X_DRAG = 10000;
const float Player::JUMP_TIME = 300;
const std::string Player::DEFAULT_PLAYER_TEXTURE = "assets/PlayerAnimation/idleRight.png";
const std::string Player::DEFAULT_ANIMATION_PATH = "assets/PlayerAnimation";
const unsigned int Player::DEFAULT_ANIMATION_FRAMES = 60;

//Path is to folder containing animation images
Player::Player(sf::Vector2f position) : 
	Entity(DEFAULT_PLAYER_TEXTURE, position),
	jumped_(false), 
	idleAnimationLeft_(Animation(DEFAULT_ANIMATION_PATH + "/idleLeft.png", 1)),
	idleAnimationRight_(Animation(DEFAULT_ANIMATION_PATH + "/idleRight.png", 1)),
	runAnimationRight_(Animation(DEFAULT_ANIMATION_PATH + "/runningRight.png", DEFAULT_ANIMATION_FRAMES)),
	runAnimationLeft_(Animation(DEFAULT_ANIMATION_PATH + "/runningLeft.png", DEFAULT_ANIMATION_FRAMES)),
	jumpAnimationLeft_(Animation(DEFAULT_ANIMATION_PATH + "/jumpLeft.png", 1)),
	jumpAnimationRight_(Animation(DEFAULT_ANIMATION_PATH + "/jumpRight.png", 1)),
	wallClingAnimationLeft_(Animation(DEFAULT_ANIMATION_PATH + "/wallClingLeft.png", 1)),
	wallClingAnimationRight_(Animation(DEFAULT_ANIMATION_PATH + "/wallClingRight.png", 1))
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
		jump_ = true;
		jumped_ = true;
		canGoHigher_ = true;
		jumpTimer_.restart();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && jumped_ && jumpTimer_.getElapsedTime().asMilliseconds() < JUMP_TIME)
	{
		if (canGoHigher_)
			jump_ = true;
	}
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W) && jumped_)
		canGoHigher_ = false;
	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && (state_ == State::WALL_CLING_LEFT || state_ == State::WALL_CLING_RIGHT))
	{
		jumped_ = true;
		if (state_ == State::WALL_CLING_LEFT)
			state_ = State::WALL_JUMP_RIGHT;
		if (state_ == State::WALL_CLING_RIGHT)
			state_ = State::WALL_JUMP_LEFT;
		return;
	}*/
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
	if (state_ == State::MOVING_LEFT || state_ == State::IDLE_LEFT)
	{
		state_ = State::IDLE_LEFT;
		return;
	}
	else
	{
		state_ = State::IDLE_RIGHT;
	}
}

void Player::render(sf::RenderWindow & window)
{
	switch (state_)
	{
	case State::IDLE_LEFT:
		runAnimationLeft_.reset();
		sprite_.setTexture(idleAnimationLeft_.getNextFrame());
		break;
	case State::IDLE_RIGHT:
		runAnimationRight_.reset();
		sprite_.setTexture(idleAnimationRight_.getNextFrame());
		break;
	case State::MOVING_LEFT:
		idleAnimationLeft_.reset();
		sprite_.setTexture(runAnimationLeft_.getNextFrame());
		break;
	case State::MOVING_RIGHT:
		idleAnimationRight_.reset();
		sprite_.setTexture(runAnimationRight_.getNextFrame());
		break;
	case State::JUMPING_LEFT:
		sprite_.setTexture(jumpAnimationLeft_.getNextFrame());
		jumpAnimationRight_.reset();
		idleAnimationLeft_.reset();
		runAnimationLeft_.reset();
		break;
	case State::JUMPING_RIGHT:
		sprite_.setTexture(jumpAnimationRight_.getNextFrame());
		jumpAnimationLeft_.reset();
		idleAnimationRight_.reset();
		runAnimationRight_.reset();
		break;
	case State::WALL_CLING_LEFT:
		sprite_.setTexture(wallClingAnimationLeft_.getNextFrame());
		idleAnimationLeft_.reset();
		break;
	case State::WALL_CLING_RIGHT:
		sprite_.setTexture(wallClingAnimationRight_.getNextFrame());
		idleAnimationRight_.reset();
		break;
	}
	sprite_.setPosition(position_);
	window.draw(sprite_);
}

void Player::handleCollision(std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	//Floor collision
	if (velocity_.y >= 0)
	{
		for (int i = 5; i < sprite_.getGlobalBounds().width - 5; i++)
		{
			if (grid[(position_.x + i) / tileBounds.x][(position_.y + sprite_.getGlobalBounds().height) / tileBounds.y].type == TileType::SOLID)
			{
				onGround_.colliding = true;
				onGround_.displacement = abs(grid[(position_.x + i) / tileBounds.x][(position_.y + sprite_.getGlobalBounds().height) / tileBounds.y].entity->getPosition().y - sprite_.getGlobalBounds().height);
				velocity_.y = 0;
				position_.y = onGround_.displacement;
				if (!jump_)
					jumped_ = false;
				break;
			}
			else
				onGround_.colliding = false;
		}
	}
	else
		onGround_.colliding = false;
	//Top platform collision
	if (velocity_.y < 0)
	{
		for (int i = 5; i < sprite_.getGlobalBounds().width - 5; i++)
		{
			if (grid[(position_.x + i) / tileBounds.x][(position_.y) / tileBounds.y].type == TileType::SOLID)
			{
				collideTop_.colliding = true;
				collideTop_.displacement = abs(grid[(position_.x + i) / tileBounds.x][(position_.y) / tileBounds.y].entity->getPosition().y + tileBounds.y);
				acceleration_.y = 0;
				velocity_.y = 0;
				position_.y = collideTop_.displacement;
				break;
			}
			else
				collideTop_.colliding = false;
		}
	}
	//Left wall collision
	if (velocity_.x < 0)
	{
		for (int i = 0; i < sprite_.getGlobalBounds().height; i++)
		{
			if (grid[(position_.x) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
			{
				collideLeft_.colliding = true;
				collideLeft_.displacement = abs(grid[(position_.x) / tileBounds.x][(position_.y + i) / tileBounds.y].entity->getPosition().x + tileBounds.x);
				acceleration_.x = 0;
				velocity_.x = 0;
				position_.x = collideLeft_.displacement;
				break;
			}
			else
				collideLeft_.colliding = false;
		}
	}
	//Right wall collision
	if (velocity_.x > 0)
	{
		for (int i = 0; i < sprite_.getGlobalBounds().height; i++)
		{
			if (grid[(position_.x + sprite_.getGlobalBounds().width) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
			{
				collideRight_.colliding = true;
				collideRight_.displacement = abs(grid[(position_.x + sprite_.getGlobalBounds().width) / tileBounds.x][(position_.y + i) / tileBounds.y].entity->getPosition().x - sprite_.getGlobalBounds().width);
				acceleration_.x = 0;
				velocity_.x = 0;
				position_.x = collideRight_.displacement;
				break;
			}
			else
				collideRight_.colliding = false;
		}
	}


}

void Player::handlePhysics(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	//Take input
	switch (state_)
	{
	case State::IDLE_RIGHT:
		acceleration_ = sf::Vector2f(-(abs(velocity_.x) / velocity_.x)*X_DRAG, 0);
		if (-(abs(velocity_.x) / velocity_.x) != -(abs(velocity_.x + acceleration_.x * time) / (velocity_.x + acceleration_.x * time)))
		{
			acceleration_.x = 0.0f;
			velocity_.x = 0.0f;
		}
		break;
	case State::IDLE_LEFT:
		acceleration_ = sf::Vector2f(-(abs(velocity_.x) / velocity_.x)*X_DRAG, 0);
		if (-(abs(velocity_.x) / velocity_.x) != -(abs(velocity_.x + acceleration_.x * time) / (velocity_.x + acceleration_.x * time)))
		{
			acceleration_.x = 0.0f;
			velocity_.x = 0.0f;
		}
		break;
	case State::MOVING_RIGHT:
		if (onGround_.colliding)
			acceleration_ = sf::Vector2f(MOVE_ACCELERATION, 0);
		else
			acceleration_ = sf::Vector2f(MOVE_ACCELERATION / 3, 0);
		break;
	case State::MOVING_LEFT:
		if (onGround_.colliding)
			acceleration_ = sf::Vector2f(-MOVE_ACCELERATION, 0);
		else
			acceleration_ = sf::Vector2f(-MOVE_ACCELERATION / 3, 0);
		break;
	case State::WALL_JUMP_LEFT:
		velocity_.y = JUMP_VELOCITY;
		velocity_.x = -MAX_X_SPEED;
		state_ = State::MOVING_LEFT;
		break;
	case State::WALL_JUMP_RIGHT:
		velocity_.y = JUMP_VELOCITY;
		velocity_.x = MAX_X_SPEED;
		state_ = State::MOVING_RIGHT;
		break;
	}
	//Check collision
	handleCollision(grid, tileBounds);

	if (jump_)
	{
		float j = jumpTimer_.getElapsedTime().asSeconds();
		//Magic number is scale thing for variable jump height over time
		velocity_.y = JUMP_VELOCITY * (1 / (jumpTimer_.getElapsedTime().asSeconds() + 1.7));
		jump_ = false;
	}

	//Run through physics
	if (!onGround_.colliding)
	{
		acceleration_ += sf::Vector2f(0, GRAVITY);
	}
	/*
	//Wall cling
	if ((state_ == State::MOVING_RIGHT || state_ == State::WALL_CLING_RIGHT) && collideRight_.colliding && !collideTop_.colliding && !onGround_.colliding)
	{
		//state_ = State::WALL_CLING_RIGHT;
	}
	else if ((state_ == State::MOVING_LEFT || state_ == State::WALL_CLING_LEFT) && collideLeft_.colliding && !collideTop_.colliding && !onGround_.colliding)
	{
		//state_ = State::WALL_CLING_LEFT;
	}*/

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
