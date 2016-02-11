#pragma once
#include "Entity.h"
#include <string>
class Clickeable : public Entity
{
public:
	Clickeable(const std::string path, const std::string path2, sf::Vector2f position);
	~Clickeable();
	virtual void update(float time) override;
	virtual void handleInput(sf::RenderWindow& window) override;
	bool clicked_;
private:
	bool mouseOver_;
	sf::FloatRect bounds_;
	sf::Texture hoverTexture_;
};