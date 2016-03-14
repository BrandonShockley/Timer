#include "Level.h"
#include "Error.h"
#include <pugixml\pugixml.hpp>
#include <SFML\Audio.hpp>

const float Level::ZOOM = 2.5;
const float Level::PARALLAX_MODIFIER = 30;
const float Level::BACKGROUND_SCALE = 4;

Level::Level()
{
}

Level::Level(std::string path) : completed_(false), died_(false), path_(path), alarmTriggered_(true), started_(false), musicTimer_()
{
	buffer_.loadFromFile("assets/drone/drone.wav");
	music_.openFromFile("assets/music/BOGO.wav");
	reverseMusic_.openFromFile("assets/music/BOGOReverse.wav");

	if (!lightShader_.loadFromFile("shaders/light.frag", sf::Shader::Fragment))
		fatalError("Failed to load light shader\n");

	//sound_.setLoop(true);
	sound_.setAttenuation(1);

	loadMapData(path);
	player_ = new Player(playerSpawnPoint_);
	//drone_ = new Drone({ sf::Vector2f(500,500), sf::Vector2f(10000, 1000) , sf::Vector2f(500,500), sf::Vector2f(1000, 10000)});
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
	delete drone_;
}

void Level::render(sf::RenderWindow& window)
{
	//sf::View view;
	lights_.clear();
	view.setSize(window.getDefaultView().getSize() * ZOOM);
	if (player_->getState() != State::SLIDING_LEFT && player_->getState() != State::SLIDING_RIGHT)
		view.setCenter(player_->getPosition().x, player_->getPosition().y + player_->getBounds().height / 2);
	else
		view.setCenter(player_->getPosition().x, player_->getPosition().y + player_->crouchHeight - player_->standardHeight / 2);
	window.setView(view);
	background_->render(window);
	for (std::vector<Tile> i : grid_)
	{
		for (Tile i : i)
		{
			if (i.entity != nullptr && sf::FloatRect(
					sf::Vector2f(window.getView().getCenter().x - window.getView().getSize().x,
					window.getView().getCenter().y - window.getView().getSize().y),
					window.getView().getSize() * 2.f).contains(i.entity->getPosition()))
				i.entity->render(window);
			if (i.type == TileType::LIGHT)
			{
				sf::CircleShape light;
				light.setRadius(tileSet_.tileWidth * 2.5);
				light.setOrigin(light.getRadius() / 2, light.getRadius() / 2);
				light.setPosition(i.entity->getPosition().x - i.entity->getBounds().width / 1.35, i.entity->getPosition().y - i.entity->getBounds().height / 1.35);
				printf("%f\n", (view.getCenter().x - view.getSize().x / 2));
				
				lights_.push_back(light);
			}
		}
	}
	
	drone_->render(window);
	if (alarmTriggered_)
	{
		for (sf::CircleShape light : lights_)
		{
			lightShader_.setParameter("worldLocation", sf::Vector2f(light.getPosition().x + light.getRadius() / 2, light.getPosition().y + light.getRadius() / 2));
			lightShader_.setParameter("viewLocation", sf::Vector2f(view.getCenter().x - view.getSize().x / 2, view.getCenter().y - view.getSize().y / 2));
			lightShader_.setParameter("zoom", ZOOM);
			lightShader_.setParameter("windowHeight", window.getSize().y);
			lightShader_.setParameter("time", time_.getElapsedTime().asMilliseconds());
			lightShader_.setParameter("radius", light.getRadius());
			lightShader_.setParameter("tileHeight", tileSet_.tileHeight);
			window.draw(light, &lightShader_);
		}
	}
	player_->render(window);
}

void Level::update(float time)
{
	if (!started_)
	{
		restart();
		started_ = true;
	}
	if (player_->restarting && !drone_->restarting)
		drone_->restarting = true;
	else if (!player_->restarting && drone_->restarting)
		player_->restarting = true;
	player_->update(time, grid_, sf::Vector2i(tileSet_.tileWidth, tileSet_.tileHeight));
	drone_->nextLocation_ = sf::Vector2f(player_->getPosition().x + player_->getBounds().width / 2, player_->getPosition().y + player_->getBounds().height / 2);
	drone_->update(time);
	//Parallax effect
	if (player_->getState() != State::SLIDING_LEFT && player_->getState() != State::SLIDING_RIGHT)
		background_->setPosition(sf::Vector2f(player_->getPosition().x - background_->getBounds().width / 3 - player_->getPosition().x / PARALLAX_MODIFIER,
			player_->getPosition().y - background_->getBounds().height / 3 - player_->getPosition().y / PARALLAX_MODIFIER));
	else
		background_->setPosition(sf::Vector2f(player_->getPosition().x - background_->getBounds().width / 3 - player_->getPosition().x / PARALLAX_MODIFIER,
			player_->getPosition().y + player_->crouchHeight - player_->standardHeight - background_->getBounds().height / 3 - (player_->getPosition().y + player_->crouchHeight - player_->standardHeight) / PARALLAX_MODIFIER));
	checkComplete();
	sf::Listener::setPosition(player_->getPosition().x + player_->getBounds().width / 2, 0, player_->getPosition().y + player_->getBounds().height / 2);
	if (sound_.getStatus() != sf::SoundSource::Status::Playing && !died_ && !completed_)
	{
		sound_.setBuffer(buffer_);
		sound_.play();
	}
	else if (died_ || completed_)
		stopSounds();
	sound_.setPosition(drone_->getPosition().x, 0, drone_->getPosition().y);
	sound_.setMinDistance(500.f);

	//if (music_.getStatus() != sf::SoundSource::Playing && reverseMusic_.getStatus() != sf::SoundSource::Playing)
		//music_.play();

	if (music_.getStatus() == sf::SoundSource::Playing)
		musicTimer_ = music_.getPlayingOffset();
	else if (reverseMusic_.getStatus() == sf::SoundSource::Playing)
		musicTimer_ = reverseMusic_.getPlayingOffset();

	if (!died_ && !completed_)
	{
		if (player_->timeTraveling_ && music_.getStatus() == sf::SoundSource::Playing)
		{
			music_.pause();
			reverseMusic_.setPlayingOffset(reverseMusic_.getDuration() - music_.getPlayingOffset());
			reverseMusic_.play();
		}
		else if (!player_->timeTraveling_ && reverseMusic_.getStatus() == sf::SoundSource::Playing)
		{
			music_.setPlayingOffset(music_.getDuration() - reverseMusic_.getPlayingOffset());
			reverseMusic_.pause();
			music_.play();
		}
	}
	else if (completed_)
		stopSounds();
}

void Level::handleInput(sf::RenderWindow & window)
{
	player_->handleInput(window);
	drone_->handleInput(window);
}

sf::Vector2f Level::getPlayerSpawn()
{
	return playerSpawnPoint_;
}

bool Level::isCompleted()
{
	return completed_;
}

bool Level::isDead()
{
	return died_;
}

std::string Level::getPath()
{
	return path_;
}

void Level::restart()
{
	/*delete(player_);
	delete(drone_);
	player_ = new Player(playerSpawnPoint_);
	drone_ = new Drone(vec);*/
	player_->restarting = true;
	drone_->restarting = true;
}

void Level::stopSounds()
{
	sound_.stop();
	music_.stop();
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
		}
	}
	//Grabs end point
	for (pugi::xml_node n : doc.child("map").children("objectgroup"))
	{
		std::string name = std::string(n.attribute("name").as_string());
		if (name == "Finish")
		{
			finishPoint_.x = n.child("object").attribute("x").as_float();
			finishPoint_.y = n.child("object").attribute("y").as_float();
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

void Level::checkComplete()
{
	float x = player_->getBounds().left;
	float y = player_->getBounds().top;
	float w = player_->getBounds().width;
	float h = player_->getBounds().height;

	if (x < finishPoint_.x && y < finishPoint_.y && x + w > finishPoint_.x && y + h > finishPoint_.y)
		completed_ = true;

	if (player_->getBounds().contains(drone_->getBounds().left, drone_->getBounds().top) ||
		player_->getBounds().contains(drone_->getBounds().left, drone_->getBounds().top + drone_->getBounds().height) ||
		player_->getBounds().contains(drone_->getBounds().left + drone_->getBounds().width, drone_->getBounds().top) ||
		player_->getBounds().contains(drone_->getBounds().left + drone_->getBounds().width, drone_->getBounds().top + drone_->getBounds().height) ||
		player_->getPosition().y + player_->getBounds().height > mapHeight_ * tileSet_.tileHeight && !player_->restarting)
		died_ = true;
	else
		died_ = false;
}
