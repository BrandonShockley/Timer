#pragma once
#include <SFML\Graphics.hpp>
#include <string>

class Entity
{
public:
	Entity(const std::string path);
	~Entity();
	virtual void update();
	virtual void render(sf::RenderWindow& window);
private:
	sf::Sprite sprite_;
	sf::Vector2f position_;
};

