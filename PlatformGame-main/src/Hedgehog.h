#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Sprite.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Hedgehog : public Enemy
{
public:

	Hedgehog();
	virtual ~Hedgehog();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void Shoot();

private:
	Timer attackTimer;
	int attackTime = 2;
};
