#include "Entity.h"
#include "Error.h"

Entity::Entity(const std::string path, sf::Vector2f position, sf::IntRect rect, float scale) : position_(position)
{
	if (rect != sf::IntRect())
	{
		if (!texture_.loadFromFile(path, rect))
		{
			fatalError("Unable to load texture: " + path);
		}
	}
	else
	{
		if (!texture_.loadFromFile(path))
		{
			fatalError("Unable to load texture: " + path);
		}
	}
	sprite_.setTexture(texture_);
	sprite_.scale(scale, scale);
	sprite_.setPosition(position_);
}

Entity::~Entity()
{
}

void Entity::update(float time)
{
	
}

void Entity::render(sf::RenderWindow& window)
{
	sprite_.setPosition(position_);
	window.draw(sprite_);
}

void Entity::handleInput(sf::RenderWindow & window)
{
}

bool Entity::hasTexture()
{
	if (sprite_.getTexture() != nullptr)
		return true;
	return false;
}

sf::Vector2f Entity::getPosition()
{
	return position_;
}

sf::FloatRect Entity::getBounds()
{
	return sprite_.getGlobalBounds();
}

void Entity::setPosition(sf::Vector2f position)
{
	position_ = position;
}
