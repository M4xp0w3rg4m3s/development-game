#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Sprite.h"
#include "Module.h"

class GameHUD : public Module {
public:
	GameHUD();
	~GameHUD() {};

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void StartInternalTimer();
	void UpdateInternalTimer();
	double ReadInternalTimerSec() const;
	std::string ReadInternalTimerFormat() const;

private:
	float internalDt;

	SDL_Texture* ignisTexture = nullptr;
	Sprite* ignisAnimator = nullptr;
	int ignis_texW = 32, ignis_texH = 32;
	int ignis_frameNumX = 5, ignis_frameNumY = 8;

	SDL_Texture* heartTexture = nullptr;
	Sprite* heartAnimator = nullptr;
	int heart_texW = 32, heart_texH = 32;
	int heart_frameNumX = 4, heart_frameNumY = 2;

	SDL_Texture* keysMenuTexture;
	SDL_Texture* lifeHudTexture;
	SDL_Texture* ignisHudTexture;

	bool keysMenuOn = false;
	bool lifeHud = false;

	double internalTimer = 0;
};