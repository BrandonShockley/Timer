#include "Cutscene.h"
#include <pugixml\pugixml.hpp>
#include "Error.h"


Cutscene::Cutscene(const std::string path)
{
	buffer_.loadFromFile("assets/drone/drone.wav");
	if (!lightShader_.loadFromFile("shaders/light.frag", sf::Shader::Fragment))
		fatalError("Failed to load light shader\n");

	loadMapData(path);
	player_ = new Player(playerSpawnPoint_);
}


Cutscene::~Cutscene()
{
}

void Cutscene::handleInput(sf::RenderWindow & window)
{

}

void Cutscene::update(float time)
{
	player_->update(time, grid_, sf::Vector2i(tileSet_.tileWidth, tileSet_.tileHeight));
	//Parallax effect
	if (player_->getState() != State::SLIDING_LEFT && player_->getState() != State::SLIDING_RIGHT)
		background_->setPosition(sf::Vector2f(player_->getPosition().x - background_->getBounds().width / 3 - player_->getPosition().x / PARALLAX_MODIFIER,
			player_->getPosition().y - background_->getBounds().height / 3 - player_->getPosition().y / PARALLAX_MODIFIER));
	else
		background_->setPosition(sf::Vector2f(player_->getPosition().x - background_->getBounds().width / 3 - player_->getPosition().x / PARALLAX_MODIFIER,
			player_->getPosition().y + player_->crouchHeight - player_->standardHeight - background_->getBounds().height / 3 - (player_->getPosition().y + player_->crouchHeight - player_->standardHeight) / PARALLAX_MODIFIER));
	checkComplete();
}

void Cutscene::loadMapData(const std::string path)
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
		}
	}

	//Grabs drone spawn
	for (pugi::xml_node n : doc.child("map").children("objectgroup"))
	{
		std::string name = std::string(n.attribute("name").as_string());
		if (name == "DroneSpawn")
		{

			for (pugi::xml_node i : n)
			{
				vec.push_back(sf::Vector2f(i.attribute("x").as_float(), i.attribute("y").as_float()));
			}
			drone_ = new Drone(vec[0]);
		}
	}

	//Grabs walk points (special cutscene stuff)
	for (pugi::xml_node n : doc.child("map").children("objectgroup"))
	{
		std::string name = std::string(n.attribute("name").as_string());
		if (name == "WalkPoints")
		{
			for (pugi::xml_node j : n.child("object"))
			{
				walkPoints_.push_back(WalkPoint(sf::Vector2f(j.attribute("x").as_float(), j.attribute("y").as_float()), 
					j.child("properties").child("property").attribute("value").as_float()));
			}
		}
	}

	//Grabs background name
	for (pugi::xml_node n : doc.child("map").child("properties").children("property"))
	{
		if (std::string(n.attribute("name").as_string()) == "background")
		{
			background_ = new Entity(std::string(n.attribute("value").as_string()), sf::Vector2f(0, 0), sf::IntRect(), BACKGROUND_SCALE);
		}
	}

	//Constructs the level
	for (pugi::xml_node n : doc.child("map").children("layer"))
	{
		pugi::xml_node l = n.child("data").child("tile");
		for (int i = 0; i < mapHeight_; i++)
		{
			for (int j = 0; j < mapWidth_; j++)
			{
				//Gid 0 = Air
				if (l.attribute("gid").as_int() == 0)
				{
					grid_[j][i].type = TileType::NONSOLID;
				}
				else if (l.attribute("gid").as_int() == 9)
				{
					//Grabs corresponding tile set coordinate
					int gid = l.attribute("gid").as_int() - 1;
					int y = (int)floor(gid / (tileSet_.imageWidth / tileSet_.tileWidth)) * tileSet_.tileHeight;
					int x = (gid - (y / tileSet_.tileWidth * (tileSet_.imageWidth / tileSet_.tileWidth))) * tileSet_.tileWidth;

					//Specifies tile type at coordinate
					grid_[j][i].type = TileType::LIGHT;
					//Specifies tile entity
					grid_[j][i].entity = new Entity(tileSet_.source, sf::Vector2f((float)(j * tileSet_.tileWidth), (float)(i * tileSet_.tileHeight)),
						sf::IntRect(x, y, tileSet_.tileWidth, tileSet_.tileHeight));
				}
				else if (l.attribute("gid").as_int() == 4 || l.attribute("gid").as_int() == 5 || l.attribute("gid").as_int() == 7 || l.attribute("gid").as_int() == 8 || l.attribute("gid").as_int() == 9 || l.attribute("gid").as_int() == 10 || l.attribute("gid").as_int() == 11)
				{
					//Grabs corresponding tile set coordinate
					int gid = l.attribute("gid").as_int() - 1;
					int y = (int)floor(gid / (tileSet_.imageWidth / tileSet_.tileWidth)) * tileSet_.tileHeight;
					int x = (gid - (y / tileSet_.tileWidth * (tileSet_.imageWidth / tileSet_.tileWidth))) * tileSet_.tileWidth;

					//Specifies tile type at coordinate
					grid_[j][i].type = TileType::NONSOLID;
					//Specifies tile entity
					grid_[j][i].entity = new Entity(tileSet_.source, sf::Vector2f((float)(j * tileSet_.tileWidth), (float)(i * tileSet_.tileHeight)),
						sf::IntRect(x, y, tileSet_.tileWidth, tileSet_.tileHeight));
				}
				else
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
				l = l.next_sibling("tile");
			}
		}
	}
}
