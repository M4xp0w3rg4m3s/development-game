#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Sprite.h"
#include "Pathfinding.h"
#include <box2d/box2d.h>

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

	void GoToPath() override;

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

private:
	Timer attackTimer;
	int attackTime = 2;

	int audioShurikenHitId = -1;
};
.