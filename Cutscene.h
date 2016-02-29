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
	void render(sf::RenderWindow& window) override;
private:
	
};

