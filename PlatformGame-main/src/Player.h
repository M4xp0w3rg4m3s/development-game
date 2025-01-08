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

	Vector2D GetCenterPosition();

	void Shoot();

	bool IsAttackingLeft() const;
	bool IsAttackingRight() const;

	void SetAttackingLeft(bool isAttackingLeft_);
	void SetAttackingRight(bool isAttackingRight_);

	int width = 16;
	std::string textureName;

	int GetPlayerLives() const;
	int GetPlayerIgnis() const;

private:

	//Texture
	SDL_Texture* texture = NULL;
	int texW = 0, texH = 0;
	Sprite* animator = nullptr;

	int height = 45;

	//Physics
	bool isGrounded = false;
	PhysBody* body = nullptr;
	PhysBody* bodyBot = nullptr;
	PhysBody* bodyAttackLeft = nullptr;
	PhysBody* bodyAttackRight = nullptr;

	//States
	PlayerState state = IDLE;

	Timer deadTimer;
	int deadTime = 2;

	//Attack
	Timer attackReactionTimer;
	Timer attackCooldownTimer;
	int reactionTimeMs = 500;
	int attackCooldown = 2000;

	Timer attackShurikenTimer;
	int attackShurikenTime = 2000;

	Timer hitTimer;
	int hitTime = 100;

	bool godMode = false;

	//Player Stats
	float speed = 150.0f;
	int lives = 3;
	int currentIgnis = 0;

	//Enemies
	Entity* enemyAttacked = nullptr;

	bool isAttackingLeft = false;
	bool isAttackingRight = false;

	//Items
	bool shurikenEnabled = false;

	Timer enabledShurikenTimer;
	int enabledShurikenTime = 60000;

	//Audio
	int audioPlayerStepsGrassId = -1;
	int audioPlayerStepsRockId = -1;
	int audioPlayerStepsId = -1;
	int audioPlayerSwordId = -1;
	int audioShurikenShootId = -1;
	int audioPlayerSwordSwingId = -1;
	int audioPlayerHurtId = -1;
	int audioPlayerDieId = -1;
	int audioHitEnemyId = -1;
	int audioFallToWaterId = -1;
	int pickCoinFxId = -1;
};