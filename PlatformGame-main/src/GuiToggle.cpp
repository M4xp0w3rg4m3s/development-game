#include "GuiToggle.h"
#include "Input.h"
#include "Render.h"
#include "Engine.h"

GuiToggle::GuiToggle(int id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::TOGGLE, id)
{
    this->bounds = bounds;
    this->text = text;

    canClick = true;
    drawBasic = false;
    inTransition = false;  

    startColor = { 113, 0, 14, 255 }; // Red
    endColor = { 7, 107, 0, 255 };   // Green
    transitionTime = 0.5f;  
    currentTime = 0.0f;   
}

bool GuiToggle::Update(float dt)
{
    if (state != GuiControlState::DISABLED)
    {
        Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

        if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

            Engine::GetInstance().render->DrawRectangle({ bounds.x - 3, bounds.y - 3, bounds.w + 6, bounds.h + 6}, 250, 248, 246, 255, true, false);

            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !inTransition) {
                if (state == GuiControlState::PRESSED)
                {
                    state = GuiControlState::NORMAL;
                    startColor = { 7, 107, 0, 255 }; // Green
                    endColor = { 113, 0, 14, 255 };   // Red
                    currentTime = 0.0f;               
                    inTransition = true;              
                }
                else if (state == GuiControlState::NORMAL)
                {
                    state = GuiControlState::PRESSED;
                    startColor = { 113, 0, 14, 255 }; // Red
                    endColor = { 7, 107, 0, 255 };   // Green
                    currentTime = 0.0f;              
                    inTransition = true;              
                }
            }
        }

        SmoothTransition(dt);

        Engine::GetInstance().render->DrawRectangle(bounds, startColor.r, startColor.g, startColor.b, startColor.a, true, false);
        Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h);
    }

    return false;
}

void GuiToggle::SmoothTransition(float dt)
{
    currentTime += dt;
    if (currentTime > transitionTime)
    {
        currentTime = transitionTime;
        inTransition = false;  
    }

    float t = currentTime / transitionTime;

    startColor.r = static_cast<Uint8>(startColor.r + t * (endColor.r - startColor.r));
    startColor.g = static_cast<Uint8>(startColor.g + t * (endColor.g - startColor.g));
    startColor.b = static_cast<Uint8>(startColor.b + t * (endColor.b - startColor.b));
    startColor.a = static_cast<Uint8>(startColor.a + t * (endColor.a - startColor.a));
}
