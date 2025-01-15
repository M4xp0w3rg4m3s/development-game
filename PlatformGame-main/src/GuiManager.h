#pragma once

#include "Module.h"
#include "GuiControl.h"

#include <list>

class GuiManager : public Module
{
public:

	GuiManager();

	virtual ~GuiManager();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	GuiControl* CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds = { 0,0,0,0 });

	bool DeleteButton(int id);

	bool DeleteButtons();

	bool DisableButton(int id);
	bool DisableButtons();
	bool EnableButtons();

public:

	std::list<GuiControl*> guiControlsList;
	SDL_Texture* texture = nullptr;

};
