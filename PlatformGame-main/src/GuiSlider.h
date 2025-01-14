#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlButton;

class GuiSlider : public GuiControl
{
public:
    GuiSlider(int id, SDL_Rect bounds, const char* text, Module* observer, float minValue, float maxValue);
    ~GuiSlider();

    bool Update(float dt) override;
    void Draw(Render* render);

    void SetValue(float valueToSet);
    float GetValue();

    SDL_Rect GetThumbBounds();
    void SetInteractionStatus(GuiControlState status);

private:
    void SelectThumb();
    void DeselectThumb();

    GuiControlButton* thumb;
    bool thumbSelected = false;
    float minVal;
    float maxVal;
    float value;
};