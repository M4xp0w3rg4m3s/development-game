#pragma once
#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlButton;

class GuiControlSlider : public GuiControl
{
public:
	GuiControlSlider(Module* moduleAt, Vector2D position, Vector2D size, Vector2D thumbSize = { 20,20 });

	~GuiControlSlider();

	bool Update(float dt);
	void SetInteractionStatus(GuiControlState status);
	float GetValue();
	void SetValue(float valueToSet);
	SDL_Rect GetThumbBounds();
	bool IsThumbSelected();

	float minVal = 0.f;
	float maxVal = 1.f;

private:
	void SelectThumb();
	void DeselectThumb();
	bool thumbSelected = false;

	float value = 0.f;
	GuiControlButton* thumb;
};