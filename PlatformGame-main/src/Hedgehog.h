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

public:

private:

	std::string textureName;
	SDL_Texture* texture = NULL;

	int texW, texH;
	int width = 48;
	int height = 32;

	pugi::xml_node parameters;

	Sprite* animator = NULL;
	PhysBody* pbody = NULL;
	Pathfinding* pathfinding = NULL;
};
