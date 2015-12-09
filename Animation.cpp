#include "Animation.h"
#include "Error.h"

Animation::Animation(const std::string path, const int frames)
{
	sf::Image img;
	if (!img.loadFromFile(path))
		fatalError("Error loading image: " + path);
	sf::Vector2u size(img.getSize().x / frames, img.getSize().y);
	for (int i = 0; i < frames; i++)
	{
		sf::Texture tex;
		tex.loadFromImage(img, sf::IntRect(i*size.x, 0, size.x, size.y));
		frames_.push_back(tex);
	}
}

Animation::~Animation()
{
}

sf::Texture& Animation::getNextFrame()
{
	if (currentFrame < frames_.size())
		currentFrame++;
	else
		currentFrame = 1;
	return frames_[currentFrame - 1];
}

void Animation::reset()
{
	currentFrame = 1;
}
