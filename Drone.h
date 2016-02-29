#pragma once
#include <vector>
#include <string>
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "Entity.h"

class Drone : public Entity
{
public:
	Drone(sf::Vector2f spawnPoint);
	~Drone();
	virtual void handleInput(sf::RenderWindow& window) override;
	virtual void update(float time) override;
	void restart();
	sf::Vector2f nextLocation_;

	bool restarting;
private:
	void updateLists();

	std::vector<sf::Vector2f> locations_;
	sf::Vector2f lastLocation_;
	
	unsigned int currentLocation_;

	sf::Vector2f velocity_;
	sf::Vector2f acceleration_;

	//Awesome timey wimey stuff
	std::vector<sf::Vector2f> positionList_;
	std::vector<sf::Vector2f> velocityList_;
	std::vector<int> currentLocationList_;
	std::vector<sf::Vector2f> lastLocationList_;
	std::vector<sf::Vector2f> nextLocationList_;
	float recordTicker_;

	bool timeTraveling_;
	float playbackTicker_;

	bool reverseToggle_;
	sf::Clock exponentialReverse_;

	float angle_;

	//Sound
	sf::SoundBuffer buffer_;
	sf::Sound sound_;

	static const std::string DEFAULT_DRONE_TEXTURE;
	static const float MAX_LINEAR_SPEED;
	static const float MAX_LINEAR_ACCEL_X;
	static const float MAX_LINEAR_ACCEL_Y;
	/*
	Plan:
	Constructed with a list of coordinates that are pushed into a vector of sf::vectors
	On construction, move at certain speed to first coordinate, then second, etc.
	*/
};

