#pragma once
#include "Entity.h"
#include "Tile.h"
#include <SFML\Graphics.hpp>

enum State
{
	IDLE,
	MOVING_LEFT,
	MOVING_RIGHT,
	JUMPING
};

class Player : public Entity
{
public:
	Player();
	Player(sf::Vector2f position, const std::string path = DEFAULT_PLAYER_TEXTURE);
	~Player();

	virtual void update(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds);
	using Entity::handleInput;
	virtual void handleInput(sf::RenderWindow& window);
	void handleCollision(std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds);
private:
	void handlePhysics(float time);

	sf::Vector2f velocity_;
	sf::Vector2f acceleration_;
	State state;
	//Collider bools
	bool onGround_;
	bool collideTop_;
	bool collideLeft_;
	bool collideRight_;
	bool jumped_;

	static const float GRAVITY;
	static const float JUMP_VELOCITY;
	static const std::string DEFAULT_PLAYER_TEXTURE;
};

