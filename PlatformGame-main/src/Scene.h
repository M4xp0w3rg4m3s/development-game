#pragma once

#include "Module.h"
#include "Player.h"
#include "Parallax.h"
#include <box2d/box2d.h>

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CameraReset();

	b2World* world;

private:
	SDL_Texture* img;

	Player* player;
	Parallax* parallax;
};