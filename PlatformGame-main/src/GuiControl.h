#pragma once

#include "Input.h"
#include "Render.h"
#include "Module.h"
#include "Engine.h"
#include "Textures.h"
#include <vector>

#include "Vector2D.h"

enum class GuiControlType
{
	BUTTON,
	TOGGLE,
	CHECKBOX,
	SLIDER,
	SLIDERBAR,
	COMBOBOX,
	DROPDOWNBOX,
	INPUTBOX,
	VALUEBOX,
	SPINNER
};

enum class GuiControlState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED
};

class GuiControl
{
public:

	GuiControl(GuiControlType type, int id) : type(type), id(id), state(GuiControlState::NORMAL) {}

	GuiControl(GuiControlType type, SDL_Rect bounds, const char* text) :
		type(type),
		state(GuiControlState::NORMAL),
		bounds(bounds),
		text(text)
	{
		color.r = 255; color.g = 255; color.b = 255;
		texture = NULL;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	void CleanUp() 
	{
		if (observer != nullptr)
		{
			delete observer;
		}
	}
	
	void SetTexture(SDL_Texture* tex)
	{
		texture = tex;
		section = { 0, 0, 0, 0 };
	}

	void SetObserver(Module* module)
	{
		observer = module;
	}

	void NotifyObserver()
	{
		observer->OnGuiMouseClickEvent(this);
	}

public:

	int id = 0;
	GuiControlType type;
	GuiControlState state;

	std::string text;           // Control text (if required)
	SDL_Rect bounds = {0,0,0,0};        // Position and size
	SDL_Color color = { 255, 255, 255, 255 };        // Tint color

	SDL_Texture* texture = nullptr;   // Texture atlas reference
	SDL_Rect section = {0,0,0,0};       // Texture atlas base section

	Module* observer = nullptr;        // Observer 
};