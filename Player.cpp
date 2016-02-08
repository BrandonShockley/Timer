#include "Player.h"
#include "Error.h"

const float Player::GRAVITY = 9000;
const float Player::SLIDE_GRAVITY = 4500;
const float Player::JUMP_VELOCITY = -3300;
const float Player::MOVE_ACCELERATION = 10000;
const float Player::MAX_X_SPEED = 2000;
const float Player::MAX_Y_SPEED = 4000;
const float Player::X_DRAG = 9000;
const float Player::JUMP_TIME = 300;
const float Player::DROP_TIME = 300;
//Every 300 ticks, update the time list
<<<<<<< HEAD
const float Player::RECORD_INTERVAL = 350;
const float Player::PLAYBACK_INTERVAL = 300;
=======
const float Player::RECORD_INTERVAL = 300;
const float Player::PLAYBACK_INTERVAL = 750;
>>>>>>> origin/master
const std::string Player::DEFAULT_PLAYER_TEXTURE = "assets/PlayerAnimation/idleRight.png";
const std::string Player::DEFAULT_ANIMATION_PATH = "assets/PlayerAnimation";
const unsigned int Player::DEFAULT_ANIMATION_FRAMES = 60;

//Path is to folder containing animation images
Player::Player(sf::Vector2f position) : 
	Entity(DEFAULT_PLAYER_TEXTURE, position),
	jumped_(false),
	startDrop_(false),
	recordTicker_(0),
	timeTraveling_(false),
	playbackTicker_(0),
	idleAnimationLeft_(Animation(DEFAULT_ANIMATION_PATH + "/idleLeft.png", 1)),
	idleAnimationRight_(Animation(DEFAULT_ANIMATION_PATH + "/idleRight.png", 1)),
	runAnimationRight_(Animation(DEFAULT_ANIMATION_PATH + "/runningRight.png", DEFAULT_ANIMATION_FRAMES)),
	runAnimationLeft_(Animation(DEFAULT_ANIMATION_PATH + "/runningLeft.png", DEFAULT_ANIMATION_FRAMES)),
	jumpAnimationLeft_(Animation(DEFAULT_ANIMATION_PATH + "/jumpLeft.png", 1)),
	jumpAnimationRight_(Animation(DEFAULT_ANIMATION_PATH + "/jumpRight.png", 1)),
	wallClingAnimationLeft_(Animation(DEFAULT_ANIMATION_PATH + "/wallClingLeft.png", 1)),
	wallClingAnimationRight_(Animation(DEFAULT_ANIMATION_PATH + "/wallClingRight.png", 1))
{
	if (!shader.loadFromFile("shaders/player.frag", sf::Shader::Fragment))
		fatalError("Failed to load player shader");

}

Player::~Player()
{
}

void Player::update(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	handlePhysics(time, grid, tileBounds);
	recordTicker_++;
	if (recordTicker_ >= RECORD_INTERVAL && !timeTraveling_)
	{
		updateLists();
		recordTicker_ = 0;
	}	
}

void Player::handleInput(sf::RenderWindow & window)
{
	//Time traveling! :D
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		timeTraveling_ = true;
		return;
	}
	else
		timeTraveling_ = false;
	//Jumping
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && onGround_.colliding && !wPress_)
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
	if ((!sf::Keyboard::isKeyPressed(sf::Keyboard::W) && jumped_) || collideTop_.colliding)
	{
		canGoHigher_ = false;
		collideTop_.colliding = false;
	}

	//Wall jump input
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && state_ == State::WALL_CLING_RIGHT && !wPress_)
	{
		jump_ = true;
		jumped_ = true;
		canGoHigher_ = true;
		jumpTimer_.restart();
		state_ = State::WALL_JUMP_LEFT;
		return;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && state_ == State::WALL_CLING_LEFT && !wPress_)
	{
		jump_ = true;
		jumped_ = true;
		canGoHigher_ = true;
		jumpTimer_.restart();
		state_ = State::WALL_JUMP_RIGHT;
		return;
	}

	//Peel off input
	//Right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && state_ == State::WALL_CLING_RIGHT && !startDrop_)
	{
		dropTimer_.restart();
		startDrop_ = true;
		return;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && state_ == State::WALL_CLING_RIGHT && startDrop_ && dropTimer_.getElapsedTime().asMilliseconds() > DROP_TIME)
	{
		state_ = State::MOVING_LEFT;
		position_.x -= 1;
		startDrop_ = false;
		return;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && state_ == State::WALL_CLING_RIGHT && startDrop_ && dropTimer_.getElapsedTime().asMilliseconds() < DROP_TIME)
		return;
	//Left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && state_ == State::WALL_CLING_LEFT && !startDrop_)
	{
		dropTimer_.restart();
		startDrop_ = true;
		return;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && state_ == State::WALL_CLING_LEFT && startDrop_ && dropTimer_.getElapsedTime().asMilliseconds() > DROP_TIME)
	{
		state_ = State::MOVING_RIGHT;
		position_.x += 1;
		startDrop_ = false;
		return;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && state_ == State::WALL_CLING_LEFT && startDrop_ && dropTimer_.getElapsedTime().asMilliseconds() < DROP_TIME)
		return;
	startDrop_ = false;

	//Prevents constant jumping from held down W
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		wPress_ = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		wPress_ = false;
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
	if (state_ == State::WALL_CLING_LEFT && collideRight_.colliding)
	{
		return;
	}
	if (state_ == State::WALL_CLING_RIGHT && collideLeft_.colliding)
	{
		return;
	}
	if (state_ == State::MOVING_LEFT || state_ == State::IDLE_LEFT || state_ == State::WALL_CLING_RIGHT)
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
	shader.setParameter("timeWarp", timeTraveling_);
	shader.setParameter("texture", *sprite_.getTexture());
	sprite_.setPosition(position_);
	window.draw(sprite_, &shader);
}

void Player::handleCollision(std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	onGround_.colliding = false;
	collideLeft_.colliding = false;
	collideRight_.colliding = false;
	collideTop_.colliding = false;
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
	if (velocity_.x <= 0)
	{
		for (int i = 0; i < sprite_.getGlobalBounds().height; i++)
		{
			if (grid[(position_.x - 1) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
			{
				collideLeft_.colliding = true;
				if (grid[(position_.x) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
				{
					collideLeft_.displacement = abs(grid[(position_.x) / tileBounds.x][(position_.y + i) / tileBounds.y].entity->getPosition().x + tileBounds.x);
					acceleration_.x = 0;
					velocity_.x = 0;
					position_.x = collideLeft_.displacement;
					break;
				}
			}
			else
				collideLeft_.colliding = false;
		}
	}
	//Right wall collision
	if (velocity_.x >= 0)
	{
		for (int i = 0; i < sprite_.getGlobalBounds().height; i++)
		{
			if (grid[(position_.x + sprite_.getGlobalBounds().width + 1) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
			{
				collideRight_.colliding = true;
				if (grid[(position_.x + sprite_.getGlobalBounds().width - 1) / tileBounds.x][(position_.y + i) / tileBounds.y].type == TileType::SOLID)
				{
					collideRight_.displacement = abs(grid[(position_.x + sprite_.getGlobalBounds().width) / tileBounds.x][(position_.y + i) / tileBounds.y].entity->getPosition().x - sprite_.getGlobalBounds().width);\
					acceleration_.x = 0;
					velocity_.x = 0;
					position_.x = collideRight_.displacement;
					break;
				}
			}
			else
				collideRight_.colliding = false;
		}
	}
}

void Player::handlePhysics(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds)
{
	if (!timeTraveling_)
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
				acceleration_ += sf::Vector2f(MOVE_ACCELERATION, 0);
			else
				acceleration_ += sf::Vector2f(MOVE_ACCELERATION / 5, 0);
			break;
		case State::MOVING_LEFT:
			if (onGround_.colliding)
				acceleration_ += sf::Vector2f(-MOVE_ACCELERATION, 0);
			else
				acceleration_ += sf::Vector2f(-MOVE_ACCELERATION / 5, 0);
			break;
		case State::WALL_JUMP_LEFT:
			velocity_.y = JUMP_VELOCITY / 1.3;
			velocity_.x = -MAX_X_SPEED / 1.3;
			state_ = State::MOVING_LEFT;
			collideRight_.colliding = false;
			break;
		case State::WALL_JUMP_RIGHT:
			velocity_.y = JUMP_VELOCITY / 1.3;
			velocity_.x = MAX_X_SPEED / 1.3;
			state_ = State::MOVING_RIGHT;
			collideLeft_.colliding = false;
			break;
		}
		//Check collision
		handleCollision(grid, tileBounds);

		//Jump :P
		if (jump_)
		{
			float j = jumpTimer_.getElapsedTime().asSeconds();
			//Magic number is scale thing for variable jump height over time
			velocity_.y = JUMP_VELOCITY * (1 / (jumpTimer_.getElapsedTime().asSeconds() + 1.7));
			jump_ = false;
		}

		//Wall slide
		if (collideRight_.colliding && state_ != State::MOVING_LEFT && !onGround_.colliding)
		{
			state_ = State::WALL_CLING_RIGHT;
		}
		else if (collideLeft_.colliding && state_ != State::MOVING_RIGHT && !onGround_.colliding)
		{
			state_ = State::WALL_CLING_LEFT;
		}

		//Run through gravity
		if (!onGround_.colliding && state_ != State::WALL_CLING_LEFT && state_ != State::WALL_CLING_RIGHT)
		{
			acceleration_ += sf::Vector2f(0, GRAVITY);
		}
		else if (state_ == State::WALL_CLING_LEFT || state_ == State::WALL_CLING_RIGHT)
		{
			acceleration_ += sf::Vector2f(0, SLIDE_GRAVITY);
		}

		velocity_ += acceleration_ * time;

		if (velocity_.x > MAX_X_SPEED)
			velocity_.x = MAX_X_SPEED;
		else if (velocity_.x < -MAX_X_SPEED)
			velocity_.x = -MAX_X_SPEED;
		if (velocity_.y > MAX_Y_SPEED)
			velocity_.y = MAX_Y_SPEED;
		else if (velocity_.y < -MAX_Y_SPEED)
			velocity_.y = -MAX_Y_SPEED;

		position_ += velocity_ * time;
		acceleration_ = sf::Vector2f(0, 0);
	}
	else 
	{
		playbackTicker_++;
		if (playbackTicker_ >= PLAYBACK_INTERVAL && positionList_.size() > 1)
		{
			position_ = positionList_[positionList_.size() - 1];
			positionList_.pop_back();
			velocity_ = velocityList_[velocityList_.size() - 1];
			velocityList_.pop_back();
			state_ = stateList_[stateList_.size() - 1];
			stateList_.pop_back();
			playbackTicker_ = 0;
		}
	}
}

void Player::updateLists()
{
	positionList_.push_back(position_);
	velocityList_.push_back(velocity_);
	stateList_.push_back(state_);
}


/*
Meat Boy Stuff
- Don't need to press in direction of wall to slide (check)
- When pressing away from wall, it takes a bit for the character to peel off
*/