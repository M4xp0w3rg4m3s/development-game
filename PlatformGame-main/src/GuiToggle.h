#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiToggle : public GuiControl
{

public:

	GuiToggle(int id, SDL_Rect bounds, const char* text);
	virtual ~GuiToggle() {}

	bool Update(float dt);

	void SmoothTransition(float dt);
private:

	bool canClick = true;
	bool drawBasic = false;

	bool inTransition = false;        // Flag to track if the transition is in progress

	SDL_Color startColor;    // Color at the beginning of the transition
	SDL_Color endColor;      // Color to transition to
	float transitionTime;    // Time to complete the transition
	float currentTime;       // Current elapsed time for smooth transition
};
