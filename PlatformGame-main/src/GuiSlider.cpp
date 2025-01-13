#include "GuiSlider.h"
#include "GuiControlButton.h"
#include "Engine.h"
#include "GuiManager.h"

GuiSlider::GuiSlider(int id, SDL_Rect bounds, const char* text, Module* observer, float minValue, float maxValue)
    : GuiControl(GuiControlType::SLIDER, id), minVal(minValue), maxVal(maxValue), value(minValue)
{
    this->bounds = bounds;
    this->text = text;
    SetObserver(observer);

    Vector2D thumbSize = { 20, 20 };
    SDL_Rect thumbPos = { bounds.x, bounds.y + bounds.h / 2 - thumbSize.getY() / 2, thumbSize.getX(), thumbSize.getY() };
    thumb = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, id + 1, "", thumbPos, observer);
}

GuiSlider::~GuiSlider()
{
    delete thumb;
}

bool GuiSlider::Update(float dt)
{
    thumb->Update(dt);

    if (thumbSelected)
    {
        Vector2D mousePos = { Engine::GetInstance().input->GetMousePosition().getX() - Engine::GetInstance().render->camera.x, Engine::GetInstance().input->GetMousePosition().getY() };
        float clampedX = std::max((float)bounds.x, std::min(mousePos.getX() - thumb->bounds.w / 2, (float)(bounds.x + bounds.w - thumb->bounds.w)));

        if (thumb->bounds.x != clampedX)
        {
            thumb->bounds.x = clampedX;
            float percent = (clampedX - bounds.x) / (bounds.w - thumb->bounds.w);
            value = minVal + percent * (maxVal - minVal);
            NotifyObserver();
        }

        if (thumb->state != GuiControlState::PRESSED)
            DeselectThumb();
    }
    else if (thumb->state == GuiControlState::PRESSED)
    {
        SelectThumb();
    }

    Draw(Engine::GetInstance().render.get());

    return true;
}

void GuiSlider::Draw(Render* render)
{
    render->DrawRectangle(bounds, 107, 124, 132, 106, true, true);

    switch (state)
    {
    case GuiControlState::DISABLED:
        Engine::GetInstance().render->DrawRectangle(thumb->bounds, 200, 200, 200, 255, true, false);
        break;
    case GuiControlState::NORMAL:
        Engine::GetInstance().render->DrawRectangle(thumb->bounds, 107, 124, 132, 106, true, false);
        break;
    case GuiControlState::FOCUSED:
        Engine::GetInstance().render->DrawRectangle(thumb->bounds, 107, 124, 132, 168, true, false);
        break;
    case GuiControlState::PRESSED:
        Engine::GetInstance().render->DrawRectangle(thumb->bounds, 200, 206, 209, 168, true, false);
        break;
    }
}

void GuiSlider::SetValue(float valueToSet)
{
    value = (valueToSet < minVal) ? minVal : (valueToSet > maxVal) ? maxVal : valueToSet;
    float percent = (value - minVal) / (maxVal - minVal);
    thumb->bounds.x = bounds.x + percent * (bounds.w - thumb->bounds.w);
}

float GuiSlider::GetValue()
{
    return value;
}

SDL_Rect GuiSlider::GetThumbBounds()
{
    return thumb->bounds;
}

void GuiSlider::SetInteractionStatus(GuiControlState status)
{
    state = status;
    thumb->state = status;
}

void GuiSlider::SelectThumb()
{
    thumbSelected = true;
}

void GuiSlider::DeselectThumb()
{
    thumbSelected = false;
}
