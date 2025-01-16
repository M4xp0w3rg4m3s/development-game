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
	void SetInternalTimer(double seconds);

	void FadeIn();
	void FadeOut();
	void ResetFadeStates();

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
	SDL_Texture* bossLifeHudTexture = nullptr;

	SDL_Texture* dieScreen = nullptr;
	SDL_Texture* lvl2Screen = nullptr;
	SDL_Texture* lvl3Screen = nullptr;

	bool keysMenuOn = false;
	bool lifeHud = false;

	double internalTimer = 0;

	//-------------- Fade ---------------//
	Timer sceneTimer;
	int sceneTime = 5 * 1000;

	bool first_fadeIn = false;
	bool first_fadeOut = false;
	bool last_fadeIn = false;
	bool last_fadeOut = false;

	//Advancing Levels
	bool begin_fadeOut = false;

	bool advance_fadeIn1 = false;
	bool advance_fadeOut1 = false;
	bool advance_fadeIn2 = false;
	bool advance_fadeOut2 = false;
	Timer advanceTimer;
	int advanceTime = 6 * 1000;

	Timer fadeTimer;
	double fadetime = 1 * 1000;
	SDL_Rect fadeRect{ 0, 0, 854, 480 };
	Uint8 opacity = 0;
	bool fadingIn = false;
	bool fadingOut = false;
};