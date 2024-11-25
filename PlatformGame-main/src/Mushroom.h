#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Sprite.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Mushroom : public Enemy
{
public:

	Mushroom();
	virtual ~Mushroom();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

public:

private:

};
