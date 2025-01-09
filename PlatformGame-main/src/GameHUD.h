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
	float internalDt = 0;

	SDL_Texture* ignisTexture = nullptr;
	Sprite* ignisAnimator = nullptr;
	int ignis_texW = 32, ignis_texH = 32;
	int ignis_frameNumX = 5, ignis_frameNumY = 8;

	SDL_Texture* heartTexture = nullptr;
	Sprite* heartAnimator = nullptr;
	int heart_texW = 32, heart_texH = 32;
	int heart_frameNumX = 4, heart_frameNumY = 2;

	int CD_texW = 54, CD_texH = 54, framesCD = 21;
	SDL_Texture* shurikenCDTexture = nullptr;
	Sprite* shurikenCDAnimator = nullptr;
	bool shurikenInCD = false;
	bool beforeShuriken = true;

	SDL_Texture* attackCDTexture = nullptr;
	Sprite* attackCDAnimator = nullptr;
	bool attackInCD = false;
	bool beforeAttack = true;

	bool playerAttacked = false;

	SDL_Texture* keysMenuTexture = nullptr;
	SDL_Texture* lifeHudTexture = nullptr;
	SDL_Texture* ignisHudTexture = nullptr;

	bool keysMenuOn = false;
	bool lifeHud = false;

	double internalTimer = 0;
};