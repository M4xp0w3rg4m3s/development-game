#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Sprite.h"

class GameHUD {
public:
	GameHUD();
	~GameHUD() {};

	bool Update(float dt);
	bool Render();
	bool CleanUp();


private:

};