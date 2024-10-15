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
		FALLING,
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


public:

	//Declare player parameters
	float speed = 150.0f;
	std::string textureName; 
	SDL_Texture* texture = NULL;
	int texW, texH;
	int width = 20;
	int height = 45;

	int pickCoinFxId;

	bool canJump = false;
	PhysBody* body;
	PhysBody* bodyBot;
	b2FixtureDef bodyBottom;

	PlayerState state = IDLE;

	Sprite* animator;
};