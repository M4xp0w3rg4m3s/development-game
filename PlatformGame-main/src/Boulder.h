#pragma once

#include "Entity.h"
#include <box2d/box2d.h>
#include "SDL2/SDL.h"

struct SDL_Texture;

class Boulder : public Entity
{
public:

	Boulder();
	virtual ~Boulder();

	bool Awake();

	bool Start();

	void SetNewPos(b2Vec2 pos);

	bool Update(float dt);

	bool CleanUp();

public:


private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;

	PhysBody* pbody;
};
