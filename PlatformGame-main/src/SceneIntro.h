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

	void FadeIn();
	void FadeOut();

	bool debug = false;

private:

	SDL_Texture* intro1 = nullptr;
	SDL_Texture* intro2 = nullptr;

	Timer imageTimer;
	int image1Time = 5 * 1000;
	Timer sceneTimer;
	int sceneTime = 10 * 1000;

	bool first_fadeIn = false;
	bool first_fadeOut = false;
	bool last_fadeIn = false;

	Timer fadeTimer;
	double fadetime = 1 * 1000;
	SDL_Rect fadeRect{ 0, 0, 854, 480 };
	Uint8 opacity = 255;
	bool fadingIn = false;
	bool fadingOut = false;
};