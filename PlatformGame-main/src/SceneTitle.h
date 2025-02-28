#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Timer.h"
#include <box2d/box2d.h>
#include "Parallax.h"

class GuiControlButton;
class GuiSlider;

class SceneTitle : public Module
{
public:

	SceneTitle();

	// Destructor
	virtual ~SceneTitle();

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

	void ButtonInteraction();

	void FadeIn();
	void FadeOut();
	void ResetFadeStates();

	void HandlePlay();
	void HandleContinue();
	void HandleCredits();

	bool debug = false;

	Parallax* parallax = nullptr;
private:
	GuiControlButton* playButton = nullptr;
	GuiControlButton* continueButton = nullptr;
	GuiControlButton* settingsButton = nullptr;
	GuiControlButton* creditsButton = nullptr;
	GuiControlButton* exitButton = nullptr;

	SDL_Texture* credits1 = nullptr;
	SDL_Texture* credits2 = nullptr;
	SDL_Texture* lvl1 = nullptr;
	SDL_Texture* lvl2 = nullptr;
	SDL_Texture* lvl3 = nullptr;
	SDL_Texture* title = nullptr;
	int titleWidth = 360;

	b2Vec2 sizeWindow = { 0,0 };

	bool playPressed = false;
	bool continuePressed = false;
	bool settingsPressed = false;
	bool creditsPressed = false;
	bool exitPressed = false;

	bool drawBg = true;
	bool saved = true;


	Timer initTimer;
	int initTime = 1 * 100;

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