#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "Parallax.h"
#include "Timer.h"

class SceneEnd : public Module
{
public:

	SceneEnd();

	// Destructor
	virtual ~SceneEnd();

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

	void FadeIn();
	void FadeOut();

private:
	Parallax* parallax = nullptr;
	SDL_Texture* theEnd = nullptr;

	int previousWidth = 0;

	Timer sceneTimer;
	int sceneTime = 30 * 1000;

	bool last_fadeIn = false;

	Timer fadeTimer;
	double fadetime = 1 * 1000;
	SDL_Rect fadeRect{ 0, 0, 854, 480 };
	Uint8 opacity = 255;
	bool fadingIn = false;
	bool fadingOut = false;
};