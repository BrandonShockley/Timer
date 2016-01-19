#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include <string>
class Animation
{
public:
	Animation(const std::string path, const int frames);
	~Animation();
	sf::Texture& getNextFrame();
	void reset();
private:
	std::vector<sf::Texture> frames_;
	unsigned int currentFrame = 1;
};