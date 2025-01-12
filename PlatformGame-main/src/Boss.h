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
	
	bool IsAttackingLeft();
	bool IsAttackingRight();

private:

	PhysBody* bodyAttackLeft = nullptr;
	PhysBody* bodyAttackRight = nullptr;

	int audioShurikenHitId = -1;

	int lives = 20;

	Timer attackTimer;
	float attackTime = 1;

	bool isAttacking = false;
	bool isAttackingLeft = false;
	bool isAttackingRight = false;
	bool fallingProjectiles = false;

	const float VELOCITY_THRESHOLD = 0.1f;
};