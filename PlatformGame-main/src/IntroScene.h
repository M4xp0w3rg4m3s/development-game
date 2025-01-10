#pragma once

#include "SDL2/SDL.h"
#include "Timer.h"
#include "Module.h"

class Sprite;

struct SDL_Texture;

class IntroScene : public Module{
private:
	SDL_Texture* texture = nullptr;

	Timer introTimer;
	int introTime = 2;

public:
	IntroScene();
	~IntroScene();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
};