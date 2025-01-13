#include "SceneSettings.h"
#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Log.h"
#include <string>

#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "GuiSlider.h"

SceneSettings::SceneSettings() : Module()
{
}

SceneSettings::~SceneSettings()
{
}

bool SceneSettings::Awake()
{
	int x = 0, y = 0;
	Engine::GetInstance().window->GetWindowSize(x, y);
	sizeWindow = { (float)x,(float)y };

	generalSlider = (GuiSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 10, "", { (int)sizeWindow.x - 200/2,(int)sizeWindow.y /3 * 1 - 10,200,20 }, this, { 0,0,0,0 });
	musicSlider = (GuiSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 11, "", { (int)sizeWindow.x - 200 / 2,(int)(sizeWindow.y / 3 * 1.5 - 10),200,20 }, this, { 0,0,0,0 });
	sfxSlider = (GuiSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 12, "", { (int)sizeWindow.x - 200 / 2,(int)sizeWindow.y / 3 * 2 - 10,200,20 }, this, { 0,0,0,0 });

	return true;
}

bool SceneSettings::Start()
{
	//intro music
	//Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");

	return true;
}

bool SceneSettings::PreUpdate()
{
	return true;
}

bool SceneSettings::Update(float dt)
{
	if (generalSlider->state == GuiControlState::PRESSED)
	{
		Engine::GetInstance().audio->ChangeGeneralVolume(generalSlider->GetValue());
	}
	else if (musicSlider->state == GuiControlState::PRESSED)
	{
		Engine::GetInstance().audio->ChangeMusicVolume(generalSlider->GetValue());
	}
	else if (sfxSlider->state == GuiControlState::PRESSED)
	{
		Engine::GetInstance().audio->ChangeSfxVolume(generalSlider->GetValue());
	}
	return true;
}

bool SceneSettings::PostUpdate()
{
	return true;
}

bool SceneSettings::CleanUp()
{
	Engine::GetInstance().guiManager->DeleteButton(10);
	Engine::GetInstance().guiManager->DeleteButton(11);
	Engine::GetInstance().guiManager->DeleteButton(12);
	return true;
}
