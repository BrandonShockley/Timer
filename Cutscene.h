#pragma once
#include "Level.h"
class Cutscene : public Level
{
public:
	Cutscene();
	~Cutscene();
	void handleInput(sf::RenderWindow& window) override;
	void update(float time) override;
	void render(sf::RenderWindow& window) override;
private:
	
};

