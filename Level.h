#pragma once
#include <string>
#include <vector>
#include <SFML\Graphics.hpp>
#include "Entity.h"
#include "Player.h"
#include "Tile.h"
#include "Drone.h"

class Level
{
public:
	Level();
	Level(const std::string path);
	~Level();

	virtual void render(sf::RenderWindow& window);
	virtual void update(float time);
	virtual void handleInput(sf::RenderWindow& window);
	sf::Vector2f getPlayerSpawn();
	bool isCompleted();
	bool isDead();
	std::string getPath();
	void restart();
	void stopSounds();
protected:
	virtual void loadMapData(const std::string path);
	//Checks if the player has reached the end
	void checkComplete();

	std::vector<sf::Vector2f> vec;
	std::string path_;

	sf::View view;

	std::vector<std::vector<Tile>> grid_;
	std::vector<sf::CircleShape> lights_;
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

	sf::Shader lightShader_;
	sf::Clock time_;

	Entity* background_;

	sf::Vector2f playerSpawnPoint_;
	sf::Vector2f finishPoint_;
	Player* player_;
	Drone* drone_;

	//Sound
	sf::SoundBuffer buffer_;
	sf::Sound sound_;
	//sf::SoundBuffer musicBuffer_;
	sf::Music music_;
	sf::Music reverseMusic_;

	sf::Time musicTimer_;

	bool completed_;
	bool died_;

	bool alarmTriggered_;

	bool started_;

	static const float ZOOM;
	static const float PARALLAX_MODIFIER;
	static const float BACKGROUND_SCALE;
};

