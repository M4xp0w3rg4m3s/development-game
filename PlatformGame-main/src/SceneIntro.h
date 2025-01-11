#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Timer.h"

class SceneIntro : public Module
{
public:

	SceneIntro();

	// Destructor
	virtual ~SceneIntro();

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

	bool debug = false;

private:

	SDL_Texture* intro1 = nullptr;
	SDL_Texture* intro2 = nullptr;

	Timer imageTimer;
	int image1Time = 3;
	Timer sceneTimer;
	int sceneTime = 5;
};