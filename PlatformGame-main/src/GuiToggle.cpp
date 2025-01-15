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
    inTransition = false;  // Initially no transition is happening

    // Initialize the transition variables
    startColor = { 200, 0, 0, 255 };  // Starting color (normal)
    endColor = { 0, 200, 0, 255 };    // Ending color (pressed)
    transitionTime = 0.5f;  // Time for a full transition (in seconds)
    currentTime = 0.0f;     // Start with no transition time elapsed
}

bool GuiToggle::Update(float dt)
{
    if (state != GuiControlState::DISABLED)
    {
        Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

        // If the position of the mouse is inside the bounds of the button
        if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

            // Only change the state when the mouse button is clicked, not held down
            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !inTransition) {
                if (state == GuiControlState::PRESSED)
                {
                    state = GuiControlState::NORMAL;
                    startColor = { 0, 200, 0, 255 }; // From pressed to normal
                    endColor = { 200, 0, 0, 255 };   // Transition to normal color
                    currentTime = 0.0f;               // Reset the timer for smooth transition
                    inTransition = true;              // Transition is now in progress
                }
                else if (state == GuiControlState::NORMAL)
                {
                    state = GuiControlState::PRESSED;
                    startColor = { 200, 0, 0, 255 }; // From normal to pressed
                    endColor = { 0, 200, 0, 255 };   // Transition to pressed color
                    currentTime = 0.0f;               // Reset the timer for smooth transition
                    inTransition = true;              // Transition is now in progress
                }
            }
        }

        // Apply the smooth transition based on elapsed time
        SmoothTransition(dt);

        // Draw the rectangle with the smooth color
        Engine::GetInstance().render->DrawRectangle(bounds, startColor.r, startColor.g, startColor.b, startColor.a, true, false);
        Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h);
    }

    return false;
}

void GuiToggle::SmoothTransition(float dt)
{
    // Update the elapsed time and clamp it to the transition time
    currentTime += dt;
    if (currentTime > transitionTime)
    {
        currentTime = transitionTime;
        inTransition = false;  // Transition is complete
    }

    // Calculate the interpolation factor (0.0 -> 1.0)
    float t = currentTime / transitionTime;

    // Linear interpolation between startColor and endColor
    startColor.r = static_cast<Uint8>(startColor.r + t * (endColor.r - startColor.r));
    startColor.g = static_cast<Uint8>(startColor.g + t * (endColor.g - startColor.g));
    startColor.b = static_cast<Uint8>(startColor.b + t * (endColor.b - startColor.b));
    startColor.a = static_cast<Uint8>(startColor.a + t * (endColor.a - startColor.a));
}
