#include "Cutscene.h"
#include <pugixml\pugixml.hpp>
#include "Error.h"


Cutscene::Cutscene(const std::string path) : currentPoint_(0), waiting_(false), droneEnabled_(false), fadeOut_(false), fadeStart_(false)
{
	alarmTriggered_ = false;
	alarmBuffer_.loadFromFile("assets/misc/alarm.wav");
	buffer_.loadFromFile("assets/drone/drone.wav");
	sound_.setAttenuation(1);
	if (!lightShader_.loadFromFile("shaders/light.frag", sf::Shader::Fragment))
		fatalError("Failed to load light shader\n");

	loadMapData(path);
	player_ = new Player(playerSpawnPoint_);
	originalPosition_ = playerSpawnPoint_;
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
	if (droneEnabled_)
	{
		drone_->nextLocation_ = sf::Vector2f(player_->getPosition().x + player_->getBounds().width / 2, player_->getPosition().y + player_->getBounds().height / 2);
		drone_->update(time);
		if (sound_.getStatus() != sf::SoundSource::Status::Playing && !died_ && !completed_)
		{
			sound_.setBuffer(buffer_);
			sound_.play();
		}
		else if (died_ || completed_)
			stopSounds();
		sound_.setPosition(drone_->getPosition().x, 0, drone_->getPosition().y);
		sound_.setMinDistance(500.f);
		if (alarm_.getStatus() != sf::SoundSource::Status::Playing && !died_ && !completed_)
		{
			alarm_.setBuffer(alarmBuffer_);
			alarm_.play();
		}
	}
	//Parallax effect
	if (player_->getState() != State::SLIDING_LEFT && player_->getState() != State::SLIDING_RIGHT)
		background_->setPosition(sf::Vector2f(player_->getPosition().x - background_->getBounds().width / 3 - player_->getPosition().x / PARALLAX_MODIFIER,
			player_->getPosition().y - background_->getBounds().height / 3 - player_->getPosition().y / PARALLAX_MODIFIER));
	else
		background_->setPosition(sf::Vector2f(player_->getPosition().x - background_->getBounds().width / 3 - player_->getPosition().x / PARALLAX_MODIFIER,
			player_->getPosition().y + player_->crouchHeight - player_->standardHeight - background_->getBounds().height / 3 - (player_->getPosition().y + player_->crouchHeight - player_->standardHeight) / PARALLAX_MODIFIER));
	checkWalk();
	if (player_->restarting)
		player_->setState(State::IDLE_RIGHT);
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
	tileTexture_.loadFromFile(tileSet_.source);

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
			for (pugi::xml_node j : n.children("object"))
			{
				float x = j.attribute("x").as_float();
				float y = j.attribute("y").as_float();
				float waitTime = j.child("properties").child("property").attribute("value").as_float();
				walkPoints_.push_back(WalkPoint(sf::Vector2f(x, y), waitTime));
			}
		}
	}

	//Grabs background name (and checks whether lights, drones, and fadeout are enabled)
	for (pugi::xml_node n : doc.child("map").child("properties").children("property"))
	{
		if (std::string(n.attribute("name").as_string()) == "background")
		{
			background_ = new Entity(std::string(n.attribute("value").as_string()), sf::Vector2f(0, 0), sf::IntRect(), BACKGROUND_SCALE);
		}
		if (std::string(n.attribute("name").as_string()) == "lightsActive")
			alarmTriggered_ = true;
		if (std::string(n.attribute("name").as_string()) == "droneActive")
			droneEnabled_ = true;
		if (std::string(n.attribute("name").as_string()) == "fadeOut")
			fadeOut_ = true;
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
					grid_[j][i].entity = new Entity(tileTexture_, sf::Vector2f((float)(j * tileSet_.tileWidth), (float)(i * tileSet_.tileHeight)),
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
					grid_[j][i].entity = new Entity(tileTexture_, sf::Vector2f((float)(j * tileSet_.tileWidth), (float)(i * tileSet_.tileHeight)),
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
					grid_[j][i].entity = new Entity(tileTexture_, sf::Vector2f((float)(j * tileSet_.tileWidth), (float)(i * tileSet_.tileHeight)),
						sf::IntRect(x, y, tileSet_.tileWidth, tileSet_.tileHeight));
				}
				l = l.next_sibling("tile");
			}
		}
	}
}

void Cutscene::checkWalk()
{
	if (waiting_)
	{
		if (player_->getState() == State::MOVING_LEFT)
			player_->setState(State::IDLE_LEFT);
		else if (player_->getState() == State::MOVING_RIGHT)
			player_->setState(State::IDLE_RIGHT);
		if (walkTimer_.getElapsedTime().asSeconds() > walkPoints_[currentPoint_].waitTime)
		{
			waiting_ = false;
			currentPoint_++;
		}
		return;
	}
	if (originalPosition_.x > walkPoints_[currentPoint_].position.x)
	{
		player_->setState(State::MOVING_LEFT);
		if (player_->getPosition().x < walkPoints_[currentPoint_].position.x)
		{
			walkTimer_.restart();
			waiting_ = true;
		}
	}
	else if (originalPosition_.x < walkPoints_[currentPoint_].position.x)
	{
		player_->setState(State::MOVING_RIGHT);
		if (player_->getPosition().x < walkPoints_[currentPoint_].position.x)
		{
			walkTimer_.restart();
			waiting_ = true;
		}
	}
	if (currentPoint_ == walkPoints_.size())
	{
		if (!fadeOut_)
			completed_ = true;
		if (!fadeStart_ && fadeOut_)
		{
			fadeStart_ = true;
			fadeTimer_.restart();
			restart();
		}
		else if (fadeStart_ && fadeTimer_.getElapsedTime().asSeconds() >= 1)
			completed_ = true;
	}
		
}
