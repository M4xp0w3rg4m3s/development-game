#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"

struct SDL_Texture;

class Boulder : public Entity
{
public:

	Boulder();
	virtual ~Boulder();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

public:


private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;

	PhysBody* pbody;
};
