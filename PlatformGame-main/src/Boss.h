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

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void OnCollision(PhysBody* physA, PhysBody* physB);

	/*void GoToPath() override;*/
	
private:
	int audioShurikenHitId = -1;

	int lives = 5;
};