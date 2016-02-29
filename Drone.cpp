#include "Drone.h"
#include "Player.h"
#include <cmath>

const std::string Drone::DEFAULT_DRONE_TEXTURE = "assets/drone/drone.png";
const float Drone::MAX_LINEAR_SPEED = 4000;
const float Drone::MAX_LINEAR_ACCEL_X = 11000;
const float Drone::MAX_LINEAR_ACCEL_Y = 10000;

Drone::Drone(sf::Vector2f spawnPoint) : Entity(DEFAULT_DRONE_TEXTURE, sf::Vector2f()), 
currentLocation_(1), 
reverseToggle_(false),
timeTraveling_(false),
restarting(false)
{
	locations_.push_back(spawnPoint);
	sprite_.setScale(1.6, 1.6);
	setPosition(locations_[0]);
	lastLocation_ = locations_[0];
	nextLocation_ = locations_[0];
	sprite_.setOrigin(getBounds().width / 2, getBounds().height / 2);
}

Drone::~Drone()
{
}

void Drone::handleInput(sf::RenderWindow & window)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		timeTraveling_ = true;
	}
	else
		timeTraveling_ = false;
}

void Drone::update(float time)
{
	if (!timeTraveling_ && !restarting)
	{
		sf::FloatRect i = getBounds();
		//Set velocity to reach next checkpoint
		if (nextLocation_.x - position_.x > 0)
		{
			angle_ = atanf((nextLocation_.y - position_.y) / (nextLocation_.x - position_.x)); //Fix
			acceleration_ = sf::Vector2f(cos(angle_) * MAX_LINEAR_ACCEL_X, sin(angle_) * MAX_LINEAR_ACCEL_Y);
		}
		else
		{
			angle_ = atanf((nextLocation_.y - position_.y) / (nextLocation_.x - position_.x)); //Fix
			acceleration_ = sf::Vector2f(-cos(angle_) * MAX_LINEAR_ACCEL_X, -sin(angle_) * MAX_LINEAR_ACCEL_Y);
		}

		if (velocity_.x > MAX_LINEAR_SPEED)
			velocity_.x = MAX_LINEAR_SPEED;
		else if (velocity_.x < -MAX_LINEAR_SPEED)
			velocity_.x = -MAX_LINEAR_SPEED;
		if (velocity_.y > MAX_LINEAR_SPEED)
			velocity_.y = MAX_LINEAR_SPEED;
		else if (velocity_.y < -MAX_LINEAR_SPEED)
			velocity_.y = -MAX_LINEAR_SPEED;
		//Update vectors
		velocity_ += acceleration_ * time;
		position_ += velocity_ * time;
		printf("Position: %f, %f\nVelocity: %f, %f\n", position_.x, position_.y, velocity_.x, velocity_.y);

		recordTicker_ += time;
		if (recordTicker_ >= Player::RECORD_INTERVAL && !timeTraveling_)
		{
			updateLists();
			recordTicker_ = 0;
		}
	}
	else
	{
		playbackTicker_ += time;
		if (restarting)
		{
			if (!reverseToggle_)
			{
				exponentialReverse_.restart();
				reverseToggle_ = true;
			}
			if (playbackTicker_ >= Player::PLAYBACK_INTERVAL / (4.f * (exponentialReverse_.getElapsedTime().asSeconds())) && positionList_.size() > 1)
			{
				position_ = positionList_[positionList_.size() - 1];
				positionList_.pop_back();
				velocity_ = velocityList_[velocityList_.size() - 1];
				velocityList_.pop_back();
				playbackTicker_ = 0;
			}
		}
		else
		{
			if (playbackTicker_ >= Player::PLAYBACK_INTERVAL && positionList_.size() > 1)
			{
				position_ = positionList_[positionList_.size() - 1];
				positionList_.pop_back();
				velocity_ = velocityList_[velocityList_.size() - 1];
				velocityList_.pop_back();
				playbackTicker_ = 0;
			}
		}
		if (positionList_.size() <= 1)
		{
			restarting = false;
			reverseToggle_ = false;
		}
	}
	sprite_.setRotation(velocity_.x / 50);
}

void Drone::restart()
{
	position_ = positionList_[0];
	velocity_ = sf::Vector2f();
	positionList_.clear();
	currentLocationList_.clear();
	lastLocationList_.clear();
	nextLocationList_.clear();
	currentLocation_ = 1;
	nextLocation_ = locations_[0];
}

void Drone::updateLists()
{
	positionList_.push_back(position_);
	velocityList_.push_back(velocity_);
	currentLocationList_.push_back(currentLocation_);
	lastLocationList_.push_back(lastLocation_);
	nextLocationList_.push_back(nextLocation_);
}