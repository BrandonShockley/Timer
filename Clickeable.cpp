#include "Clickeable.h"
#include "Error.h"


Clickeable::Clickeable(const std::string path, const std::string path2, sf::Vector2f position) : Entity(path, position), mouseOver_(false), clicked_(false)
{
	if (!hoverTexture_.loadFromFile(path2))
	{
		fatalError("Failed to load file: " + path2);
	}
}

Clickeable::~Clickeable()
{
}

void Clickeable::update(float time)
{
	bounds_ = sf::FloatRect(this->sprite_.getGlobalBounds());
	if (mouseOver_)
		sprite_.setTexture(hoverTexture_);
	else
		sprite_.setTexture(texture_);
}

void Clickeable::handleInput(sf::RenderWindow & window)
{
	if (sf::Mouse::getPosition().x > bounds_.left && sf::Mouse::getPosition().x < bounds_.left + bounds_.width
		&& sf::Mouse::getPosition().y > bounds_.top && sf::Mouse::getPosition().y < bounds_.top + bounds_.height)
		mouseOver_ = true;
	else
		mouseOver_ = false;
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouseOver_)
		clicked_ = true;
	else
		clicked_ = false;
}
