#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Sprite.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Octopus : public Enemy
{
public:

	Octopus();
	virtual ~Octopus();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void Shoot();

	void GoToPath();
public:

private:
	Timer attackTimer;
	int attackTime = 2;
};
