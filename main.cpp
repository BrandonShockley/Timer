#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include "Game.h"

static const unsigned int WINDOW_WIDTH = 1080;
static const unsigned int WINDOW_HEIGHT = 720;
static const std::string WINDOW_TITLE = "Timer";

int main()
{
	sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), WINDOW_TITLE/*, sf::Style::Fullscreen*/);
	Game game(window);
	return 0;
}