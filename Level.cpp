#include "Level.h"
#include "Error.h"
#include <pugixml\pugixml.hpp>

Level::Level(std::string path)
{
	loadMapData(path);
	player_ = new Player(playerSpawnPoint_);
}

Level::~Level()
{
	for (std::vector<Tile> i : grid_)
	{
		for (Tile i : i)
		{
			delete i.entity;
		}
	}
	delete player_;
}

void Level::render(sf::RenderWindow& window)
{
	sf::View view;
	view.setSize(((float)(mapWidth_ * tileSet_.tileWidth)) / 1.5f, (float)(mapHeight_ * tileSet_.tileHeight / 2));
	view.setCenter(player_->getPosition().x, player_->getPosition().y);
	window.setView(view);
	for (std::vector<Tile> i : grid_)
	{
		for (Tile i : i)
		{
			if (i.entity != nullptr)
				i.entity->render(window);
		}
	}
	player_->render(window);
}

void Level::update(float time)
{
	player_->update(time, grid_, sf::Vector2i(tileSet_.tileWidth, tileSet_.tileHeight));
}

void Level::handleInput(sf::RenderWindow & window)
{
	player_->handleInput(window);
}

sf::Vector2f Level::getPlayerSpawn()
{
	return playerSpawnPoint_;
}

void Level::loadMapData(const std::string path)
{
	//Loads tile data
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
	{
		fatalError("XML Loading Error: " + std::string(result.description()));
	}
	pugi::xml_node tileSet = doc.child("map").child("tileset");
	tileSet_.firstgid = tileSet.attribute("firstgid").as_int();
	tileSet_.imageHeight = tileSet.child("image").attribute("height").as_int();
	tileSet_.imageWidth = tileSet.child("image").attribute("width").as_int();
	tileSet_.name = std::string(tileSet.attribute("name").as_string());
	std::string temp = std::string(tileSet.child("image").attribute("source").as_string());
	temp.erase(0, 3); //Gets rid of ../ characters of tiled file
	tileSet_.source = temp;
	tileSet_.tileHeight = tileSet.attribute("tileheight").as_int();
	tileSet_.tileWidth = tileSet.attribute("tilewidth").as_int();
	tileSet_.lastgid = (int)floor(tileSet_.imageWidth / tileSet_.tileWidth) * (int)floor(tileSet_.imageHeight / tileSet_.tileHeight);

	//Loads map data
	mapWidth_ = doc.child("map").attribute("width").as_int();
	mapHeight_ = doc.child("map").attribute("height").as_int();
	for (int i = 0; i < mapWidth_; i++)
	{
		grid_.push_back(std::vector<Tile>());
		for (int j = 0; j < mapHeight_; j++)
		{
			grid_[i].push_back(Tile());
		}
	}

	//Grabs spawn points
	for (pugi::xml_node n : doc.child("map").children("objectgroup"))
	{
		std::string name = std::string(n.attribute("name").as_string());
		if (name == "PlayerSpawn")
		{
			playerSpawnPoint_.x = n.child("object").attribute("x").as_float();
			playerSpawnPoint_.y = n.child("object").attribute("y").as_float();
		}//TODO: Add chaser enemy spawn point code
	}

	//Constructs the level
	for (pugi::xml_node n : doc.child("map").children("layer"))
	{
		pugi::xml_node l = n.child("data").child("tile");
		for (int i = 0; i < mapHeight_; i++)
		{
			for (int j = 0; j < mapWidth_; j++)
			{
				if (l.attribute("gid").as_int() != 0)
				{
					//Grabs corresponding tile set coordinate
					int gid = l.attribute("gid").as_int() - 1;
					int y = (int)floor(gid / (tileSet_.imageWidth / tileSet_.tileWidth)) * tileSet_.tileHeight;
					int x = (gid - (y / tileSet_.tileWidth * (tileSet_.imageWidth / tileSet_.tileWidth))) * tileSet_.tileWidth;

					//Specifies tile type at coordinate
					grid_[j][i].type = TileType::SOLID;
					//Specifies tile entity
					grid_[j][i].entity = new Entity(tileSet_.source, sf::Vector2f((float)(j * tileSet_.tileWidth), (float)(i * tileSet_.tileHeight)), 
						sf::IntRect(x, y, tileSet_.tileWidth, tileSet_.tileHeight));
				}
				else //Gid = 0 indicates air tile
				{
					grid_[j][i].type = TileType::NONSOLID;
				}
				l = l.next_sibling("tile");
			}
		}
	}
}