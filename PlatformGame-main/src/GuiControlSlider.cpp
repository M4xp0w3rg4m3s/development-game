#include "GuiControlSlider.h"
#include "GuiControlButton.h"
#include "Engine.h"
#include "GuiManager.h"

GuiControlSlider::GuiControlSlider(Module* moduleAt, Vector2D position, Vector2D size, Vector2D thumbSize) : GuiControl(GuiControlType::SLIDER, id)
{
	bounds = { (int)position.getX(), (int)position.getY(), (int)size.getX(), (int)size.getY()};

	SDL_Rect thumbPos = { position.getX() - thumbSize.getX() / 2.f ,position.getY() - thumbSize.getY() / 2.f + size.getY() / 2,thumbSize.getX(),thumbSize.getY()};
	thumb = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "", thumbPos,observer);

	if (thumb->state == GuiControlState::PRESSED)
	{
		SelectThumb();
	}
}

GuiControlSlider::~GuiControlSlider()
{
	delete thumb;
}

bool GuiControlSlider::Update(float dt)
{
	thumb->Update(dt);

	if (thumbSelected)
	{
		Vector2D mousePos = { Engine::GetInstance().input->GetMousePosition().getX() - Engine::GetInstance().render->camera.x,
			Engine::GetInstance().input->GetMousePosition().getY() };

		mousePos.setX(Engine::GetInstance().input->GetMousePosition().getX() - thumb->bounds.w / 2.f);
		Vector2D moveBounds = { bounds.x - thumb->bounds.w / 2.f , bounds.x + bounds.w - thumb->bounds.w / 2.f };

		if (mousePos.getX() < moveBounds.getX())
			mousePos.setX(moveBounds.getX());
		if (mousePos.getX() > moveBounds.getY())
			mousePos.setX(moveBounds.getY());


		if (thumb->bounds.x != mousePos.getX()) {
			thumb->bounds.x = mousePos.getX();
		}

		if (thumb->state != GuiControlState::PRESSED)
			DeselectThumb();
	}
	else
	{
		DeselectThumb();
	}

	return true;
}
void GuiControlSlider::SetInteractionStatus(GuiControlState status)
{
	this->state = status;
	thumb->state = status;
}


float GuiControlSlider::GetValue()
{
	Vector2D moveBounds = { (float)(bounds.x - thumb->bounds.w / 2) , (float)(bounds.x + bounds.w - thumb->bounds.w / 2) };

	return minVal + (thumb->bounds.x - moveBounds.getX()) * (maxVal - minVal) / (moveBounds.getY() - moveBounds.getX());
}

void GuiControlSlider::SetValue(float valueToSet)
{
	value = valueToSet;

	if (value < minVal)
		value = minVal;
	if (value > maxVal)
		value = maxVal;

	Vector2D moveBounds = { (float)(bounds.x - thumb->bounds.w / 2) , (float)(bounds.x + bounds.w - thumb->bounds.w / 2) };
	thumb->bounds.x = moveBounds.getX() + (value - minVal) * (moveBounds.getY() - moveBounds.getY()) / (maxVal - minVal);

}

SDL_Rect GuiControlSlider::GetThumbBounds()
{
	return thumb->bounds;
}

bool GuiControlSlider::IsThumbSelected()
{
	return thumbSelected;
}

void GuiControlSlider::SelectThumb()
{
	thumbSelected = true;
}

void GuiControlSlider::DeselectThumb()
{
	thumbSelected = false;
}
