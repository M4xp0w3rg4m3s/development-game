#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Timer.h"
#include <box2d/box2d.h>

class GuiControlButton;

class SceneSettings : public Module 
{
private:

public:
	SceneSettings();
	~SceneSettings();
};