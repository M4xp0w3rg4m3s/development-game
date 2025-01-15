#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Timer.h"
#include <box2d/box2d.h>

class GuiControlButton;
class GuiSlider;
class GuiToggle;

class ScenePause : public Module
{
public:
	ScenePause();
	~ScenePause();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void ButtonInteraction();

	void FadeIn();
	void FadeOut();
	void ResetFadeStates();

	void HandleContinue();
private:
	GuiControlButton* resumeButton = nullptr;
	GuiControlButton* settingsButton = nullptr;
	GuiControlButton* backToTitleButton = nullptr;
	GuiControlButton* exitButton = nullptr;

	b2Vec2 sizeWindow = { 0,0 };

	bool resumePressed = false;
	bool settingsPressed = false;
	bool backToTitlePressed = false;
	bool exitPressed = false;

	bool drawBg = true;

	SDL_Texture* lvl1 = nullptr;
	SDL_Texture* lvl2 = nullptr;
	SDL_Texture* lvl3 = nullptr;

	//----------Fade----------//
	Timer imageTimer;
	int image1Time = 5 * 1000;
	Timer sceneTimer;
	int sceneTime = 10 * 1000;
	bool first_fadeIn = false;
	bool first_fadeOut = false;
	bool last_fadeIn = false;

	Timer lvlImageTimer;
	int lvlImageTime = 3 * 1000;
	bool lvlImage_fadeOut = false;

	Timer fadeTimer;
	double fadetime = 1 * 1000;
	SDL_Rect fadeRect{ 0, 0, 854, 480 };
	Uint8 opacity = 255;
	bool fadingIn = false;
	bool fadingOut = false;
}; 