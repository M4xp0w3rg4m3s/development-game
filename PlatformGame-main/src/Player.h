#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include <box2d/box2d.h>
#include "Sprite.h"

class Player : public Entity
{
public:

	enum PlayerState
	{
		IDLE = 1,
		RUNNING,
		JUMPING,
		WOMBO,
		COMBO,
		DYING,
		DEAD	
	};

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void KillPlayer();

	void ResetPlayer();

	void ResetPlayer(int level);
	
	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

public:

	//Declare player parameters
	float speed = 150.0f;
	std::string textureName; 
	SDL_Texture* texture = NULL;
	int texW, texH;
	int width = 16;
	int height = 45;

	int pickCoinFxId;

	bool isGrounded = false;
	PhysBody* body;
	PhysBody* bodyBot;

	PlayerState state = IDLE;

	Sprite* animator;

	Timer deadTimer;
	int deadTime = 2;

	Timer attackReactionTimer;
	Timer attackCooldownTimer;
	int reactionTimeMs = 500;
	int attackCooldown = 2000;

	bool godMode = false;

	int lives = 3;
};