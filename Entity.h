#pragma once
#include <SFML\Graphics.hpp>
#include <string>

class Entity
{
public:
	Entity(const std::string path, sf::Vector2f position, sf::IntRect rect = sf::IntRect(), float scale = 1);
	~Entity();
	virtual void update(float time);
	virtual void render(sf::RenderWindow& window);
	virtual void handleInput(sf::RenderWindow& window);
	bool hasTexture();
	sf::Vector2f getPosition();
protected:
	sf::Texture texture_;
	sf::Sprite sprite_;
	sf::Vector2f position_;
};