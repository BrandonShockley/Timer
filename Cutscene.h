#pragma once
#include <SFML\Graphics.hpp>
#include <string>
#include "Level.h"
class Cutscene : public Level
{
public:
	Cutscene(const std::string path);
	~Cutscene();
	void handleInput(sf::RenderWindow& window) override;
	void update(float time) override;
	//void render(sf::RenderWindow& window) override;
private:
	void loadMapData(const std::string path) override;
	void checkWalk();

	//POD for walkpoints
	struct WalkPoint {
		sf::Vector2f position;
		float waitTime;
		WalkPoint(sf::Vector2f positionx, float waitTimex) : position(positionx), waitTime(waitTimex) {};
	};
	std::vector<WalkPoint> walkPoints_;
	unsigned int currentPoint_;

	sf::Clock walkTimer_;
	bool waiting_;

	bool droneEnabled_;

	bool fadeOut_;
	bool fadeStart_;
	sf::Clock fadeTimer_;

	State originalState_;
	sf::Vector2f originalPosition_;
};

