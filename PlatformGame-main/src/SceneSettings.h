#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Timer.h"
#include <box2d/box2d.h>

class GuiControlButton;
class GuiSlider;
class GuiToggle;
class Parallax;

class SceneSettings : public Module 
{
public:
	SceneSettings();
	~SceneSettings();

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

	Parallax* parallax = nullptr;
private:
	GuiSlider* generalSlider = nullptr;
	GuiSlider* musicSlider = nullptr;
	GuiSlider* sfxSlider = nullptr;

	GuiControlButton* exitButton = nullptr;

	GuiToggle* fullscreen = nullptr;


	b2Vec2 sizeWindow = { 0,0 };

	bool exitButtonPressed = false;

	bool once = false;
	bool fullscreen_bool = true;
	bool pressed_once = false;
	bool FirstTime = true;

	bool drawBg = true;
};