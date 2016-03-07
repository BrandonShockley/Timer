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

	//POD for walkpoints
	struct WalkPoint {
		sf::Vector2f position;
		float waitTime;
		WalkPoint(sf::Vector2f position, float waitTime) : position(position), waitTime(waitTime) {};
	};
	std::vector<WalkPoint> walkPoints_;
};

