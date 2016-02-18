#include "Drone.h"
#include <cmath>

const std::string Drone::DEFAULT_DRONE_TEXTURE = "assets/drone/drone.png";
const float Drone::MAX_LINEAR_SPEED = 1000;

Drone::Drone(std::vector<sf::Vector2f> points) : Entity(DEFAULT_DRONE_TEXTURE, sf::Vector2f()), currentLocation_(1), locations_(points)
{
	setPosition(locations_[0]);
	lastLocation_ = locations_[0];
	nextLocation_ = locations_[1];
}

Drone::~Drone()
{
}

void Drone::update(float time)
{
	sf::FloatRect i = getBounds();
	//Check if at checkpoint
	if ((position_.x < nextLocation_.x) && (position_.y < nextLocation_.y) && (position_.x + i.width > nextLocation_.x) && (position_.y + i.height > nextLocation_.y))
	{
		lastLocation_ = nextLocation_;
		currentLocation_++;
		if (!(currentLocation_ >= locations_.size()))
			nextLocation_ = locations_[currentLocation_];
	}
	//Set velocity to reach next checkpoint
	if (nextLocation_.x - lastLocation_.x > 0)
	{
		angle_ = atanf((nextLocation_.y - lastLocation_.y) / (nextLocation_.x - lastLocation_.x)); //Fix
		velocity_ = sf::Vector2f(cos(angle_) * MAX_LINEAR_SPEED, sin(angle_) * MAX_LINEAR_SPEED);
	}
	else
	{
		angle_ = atanf((nextLocation_.y - lastLocation_.y) / (nextLocation_.x - lastLocation_.x)); //Fix
		velocity_ = sf::Vector2f(-cos(angle_) * MAX_LINEAR_SPEED, -sin(angle_) * MAX_LINEAR_SPEED);
	}
	//Update vectors
	velocity_ += acceleration_ * time;
	position_ += velocity_ * time;
	printf("Position: %f, %f\nVelocity: %f, %f\n", position_.x, position_.y, velocity_.x, velocity_.y);
}
