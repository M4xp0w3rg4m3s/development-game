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
    SDL_Rect thumbPos = { bounds.x, (int)(bounds.y + bounds.h / 2 - thumbSize.getY() / 2), (int)thumbSize.getX(), (int)thumbSize.getY() };
    thumb = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, id + 1, "", thumbPos, observer);

    SetValue(0.5);
}

GuiSlider::~GuiSlider()
{
    delete thumb;
}

bool GuiSlider::Update(float dt)
{
    if (state != GuiControlState::DISABLED)
    {
        thumb->Update(dt);

        if (thumbSelected)
        {
            Vector2D mousePos = { Engine::GetInstance().input->GetMousePosition().getX(), Engine::GetInstance().input->GetMousePosition().getY() };
            float clampedX = mousePos.getX() - thumb->bounds.w / 2;

            // Manually clamp clampedX within slider bounds
            if (clampedX < bounds.x) clampedX = (float)bounds.x;
            if (clampedX > bounds.x + bounds.w - thumb->bounds.w) clampedX = (float)(bounds.x + bounds.w - thumb->bounds.w);

            if (thumb->bounds.x != clampedX)
            {
                thumb->bounds.x = (int)clampedX;

                // Calculate percent and manually clamp it between 0 and 1
                float percent = (clampedX - bounds.x) / (bounds.w - thumb->bounds.w);
                if (percent < 0.0f) percent = 0.0f;
                if (percent > 1.0f) percent = 1.0f;

                // Directly assign percent to value (normalized between 0 and 1)
                value = percent;

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
    }
    return false;
}


void GuiSlider::Draw(Render* render)
{
    int adjustedX = bounds.x - Engine::GetInstance().render->camera.x;
    int adjustedY = bounds.y - Engine::GetInstance().render->camera.y;
    SDL_Rect adjustedBounds = { adjustedX, adjustedY, bounds.w, bounds.h };

    render->DrawRectangle(adjustedBounds, 107, 124, 132, 106, true, true);

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
    // Manually clamp valueToSet between 0 and 1
    if (valueToSet < 0.0f) valueToSet = 0.0f;
    if (valueToSet > 1.0f) valueToSet = 1.0f;

    value = valueToSet;

    // Update thumb position accordingly
    thumb->bounds.x = (int)(bounds.x + value * (bounds.w - thumb->bounds.w));
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
    //thumb->state = status;
}

void GuiSlider::SelectThumb()
{
    thumbSelected = true;
    SetInteractionStatus(GuiControlState::PRESSED);
}

void GuiSlider::DeselectThumb()
{
    thumbSelected = false;
    SetInteractionStatus(GuiControlState::NORMAL);
}
