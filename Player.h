#pragma once
#include "Entity.h"
#include "Tile.h"
#include "Animation.h"
#include <SFML\Graphics.hpp>
#include <vector>

enum State
{
	IDLE_LEFT,
	IDLE_RIGHT,
	MOVING_LEFT,
	MOVING_RIGHT,
	JUMPING_LEFT,
	JUMPING_RIGHT,
	WALL_CLING_LEFT,
	WALL_CLING_RIGHT,
	WALL_JUMP_LEFT,
	WALL_JUMP_RIGHT
};

struct ColliderData
{
	bool colliding;
	int displacement;
};

class Player : public Entity
{
public:
	static const float RECORD_INTERVAL;
	static const float PLAYBACK_INTERVAL;
	static const float RECORD_LIMIT;
	//Path is to folder containing animation images
	Player(sf::Vector2f position);
	~Player();

	virtual void update(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds);
	using Entity::handleInput;
	virtual void handleInput(sf::RenderWindow& window) override;
	using Entity::render;
	virtual void render(sf::RenderWindow& window) override;
	void handleCollision(std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds);
	void restart();

	bool restarting;
private:
	void handlePhysics(float time, std::vector<std::vector<Tile>> grid, sf::Vector2i tileBounds);
	void updateLists();
	
	//Key Bindings
	sf::Keyboard::Key jumpKey_;
	sf::Keyboard::Key leftKey_;
	sf::Keyboard::Key rightKey_;
	sf::Keyboard::Key timeKey_;

	//Extra
	sf::Keyboard::Key xJumpKey_;
	sf::Keyboard::Key xLeftKey_;
	sf::Keyboard::Key xRightKey_;
	sf::Keyboard::Key xTimeKey_;

	sf::Vector2f velocity_;
	sf::Vector2f acceleration_;
	State state_;
	//Collider bools
	ColliderData onGround_;
	ColliderData collideTop_;
	ColliderData collideLeft_;
	ColliderData collideRight_;
	bool jumped_;

	bool jump_;
	bool canGoHigher_;
	sf::Clock jumpTimer_;
	bool wPress_;

	sf::Clock dropTimer_;
	bool startDrop_;

	//Awesome timey wimey stuff
	std::vector<sf::Vector2f> positionList_;
	std::vector<sf::Vector2f> velocityList_;
	std::vector<State> stateList_;
	float recordTicker_;

	bool timeTraveling_;
	float playbackTicker_;

	bool restartToggle_;
	sf::Clock exponentialReverse_;

	unsigned int timeLimiter_;

	sf::RectangleShape gradient_;
	sf::RectangleShape timeBar_;

	Animation idleAnimationLeft_;
	Animation idleAnimationRight_;
	Animation runAnimationLeft_;
	Animation runAnimationRight_;
	Animation jumpAnimationLeft_;
	Animation jumpAnimationRight_;
	Animation wallClingAnimationLeft_;
	Animation wallClingAnimationRight_;

	//SHADERS!!!! :D
	sf::Shader shader_;
	sf::Shader rectShader_;
	//Used for fading effects
	sf::Clock fader_;
	bool faderBool_;

	static const float GRAVITY;
	static const float SLIDE_GRAVITY;
	static const float JUMP_VELOCITY;
	static const float MOVE_ACCELERATION;
	static const float MAX_X_SPEED;
	static const float MAX_Y_SPEED;
	static const float X_DRAG;
	static const float X_DRAG_AIR;
	static const std::string DEFAULT_PLAYER_TEXTURE;
	static const std::string DEFAULT_ANIMATION_PATH;
	static const unsigned int DEFAULT_ANIMATION_FRAMES;
	static const float JUMP_TIME;
	static const float DROP_TIME;

	static const float FADER_DURATION;
};