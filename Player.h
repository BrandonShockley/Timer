#pragma once
#include "Entity.h"
#include "Tile.h"
#include "Animation.h"
#include <SFML\Graphics.hpp>

enum State
{
	IDLE,
	MOVING_LEFT,
	MOVING_RIGHT
};

struct ColliderData
{
	bool colliding;
	int displacement;
};

class Player : public Entity
{
public:
	//Path is to folder containing animation images
	Player(sf::Vector2f position);
	~Player();

	virtual void update(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds);
	using Entity::handleInput;
	virtual void handleInput(sf::RenderWindow& window) override;
	using Entity::render;
	virtual void render(sf::RenderWindow& window) override;
	void handleCollision(std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds);
private:
	void handlePhysics(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds);

	sf::Vector2f velocity_;
	sf::Vector2f acceleration_;
	State state_;
	//Collider bools
	ColliderData onGround_;
	ColliderData collideTop_;
	ColliderData collideLeft_;
	ColliderData collideRight_;
	bool jumped_;

	Animation idleAnimation_;
	Animation runAnimation_;

	static const float GRAVITY;
	static const float JUMP_VELOCITY;
	static const float MOVE_ACCELERATION;
	static const float MAX_X_SPEED;
	static const float MAX_Y_SPEED;
	static const float X_DRAG;
	static const std::string DEFAULT_PLAYER_TEXTURE;
	static const std::string DEFAULT_ANIMATION_PATH;
	static const unsigned int DEFAULT_ANIMATION_FRAMES;
};