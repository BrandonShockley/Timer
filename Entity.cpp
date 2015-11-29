#include "Entity.h"
#include "Error.h"

Entity::Entity(const std::string path)
{
	sf::Texture texture;
	if (!texture.loadFromFile(path))
	{
		fatalError("Unable to load texture: " + path);
	}
	sprite_.setTexture(texture);
}

Entity::~Entity()
{
}

void Entity::update()
{
	
}

void Entity::render(sf::RenderWindow& window)
{
	window.draw(sprite_);
}
