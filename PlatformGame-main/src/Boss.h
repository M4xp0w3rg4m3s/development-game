#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Sprite.h"
#include "Pathfinding.h"
#include <box2d/box2d.h>

struct SDL_Texture;

class Boss : public Enemy
{
public:

	Boss();
	virtual ~Boss();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void Attack();

	void Shoot();

	void Move();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void GoToPath() override;
	
private:
	int audioShurikenHitId = -1;

	int lives = 20;

	bool IsAttacking = false;
	bool IsUp = false;

	Timer attackTimer;
	int attackTime = 1;

	bool fallingProjectiles = false;
};