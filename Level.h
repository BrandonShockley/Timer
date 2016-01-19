#pragma once
#include <string>
#include <vector>
#include <SFML\Graphics.hpp>
#include "Entity.h"
#include "Player.h"
#include "Tile.h"

class Level
{
public:
	Level(const std::string path);
	~Level();

	void render(sf::RenderWindow& window);
	void update(float time);
	void handleInput(sf::RenderWindow& window);
	sf::Vector2f getPlayerSpawn();
private:
	void loadMapData(const std::string path);

	std::vector<std::vector<Tile>> grid_;
	int mapWidth_;
	int mapHeight_;
	//Inline tile set POD
	struct 
	{
		int firstgid;
		int lastgid;
		std::string name;
		int tileWidth;
		int tileHeight;
		int imageWidth;
		int imageHeight;
		std::string source;
	} tileSet_;

	Entity* background_;

	sf::Vector2f playerSpawnPoint_;
	Player* player_;
};

