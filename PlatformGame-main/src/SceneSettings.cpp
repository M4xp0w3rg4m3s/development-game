#include "SceneSettings.h"
#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
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
	LOG("Loading TitleScene");
	bool ret = true;

	int x = 0, y = 0;
	Engine::GetInstance().window->GetWindowSize(x, y);
	sizeWindow = { (float)x,(float)y };

	generalSlider = (GuiSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 10, "", { (int)sizeWindow.x / 2 - 200 / 2,(int)sizeWindow.y / 3 * 1 - 10,200,20 }, this, { 0,0,0,0 });
	musicSlider = (GuiSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 11, "", { (int)sizeWindow.x / 2 - 200 / 2,(int)(sizeWindow.y / 3 * 1.5 - 10),200,20 }, this, { 0,0,0,0 });
	sfxSlider = (GuiSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 12, "", { (int)sizeWindow.x / 2 - 200 / 2,(int)sizeWindow.y / 3 * 2 - 10,200,20 }, this, { 0,0,0,0 });

	int exitWidth = 100, exitHeight = 25;
	SDL_Rect btExitPos = {0,0, exitWidth,exitHeight };
	exitButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "    Exit    ", btExitPos, this);

	return ret;
}

bool SceneSettings::Start()
{
	//intro music
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");
	
	return true;
}

bool SceneSettings::PreUpdate()
{
	return true;
}

bool SceneSettings::Update(float dt)
{
	bool ret = true;


	if (generalSlider->state == GuiControlState::PRESSED)
	{
		Engine::GetInstance().audio->ChangeGeneralVolume(generalSlider->GetValue());
	}
	else if (musicSlider->state == GuiControlState::PRESSED)
	{
		Engine::GetInstance().audio->ChangeMusicVolume(musicSlider->GetValue());
	}
	else if (sfxSlider->state == GuiControlState::PRESSED)
	{
		Engine::GetInstance().audio->ChangeSfxVolume(sfxSlider->GetValue());
	}
	else if (exitButton->state == GuiControlState::PRESSED)
	{
		exitButtonPressed = true;
	}

	if (exitButtonPressed)
	{
		exitButtonPressed = false;
		Engine::GetInstance().guiManager->DeleteButtons();
		if (Engine::GetInstance().previousLoopState == LoopState::GAME)
		{
			Engine::GetInstance().ChangeLoopStateWithoutStart(Engine::GetInstance().previousLoopState);
		}
		else
		{
			Engine::GetInstance().ChangeLoopState(Engine::GetInstance().previousLoopState);
		}
		
	}
	
	return ret;
}

bool SceneSettings::PostUpdate()
{
	bool ret = true;
	// Detects if the player wants to exit the game with ESC key
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool SceneSettings::CleanUp()
{
	Engine::GetInstance().guiManager->DeleteButtons();
	return true;
}
