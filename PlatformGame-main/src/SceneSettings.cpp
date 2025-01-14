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

	generalSlider->SetValue(Engine::GetInstance().audio->GetGeneralVolume());
	musicSlider->SetValue(Engine::GetInstance().audio->GetMusicVolume());
	sfxSlider->SetValue(Engine::GetInstance().audio->GetSfxVolume());

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

	//---Text---//
	Engine::GetInstance().render->DrawText("OPTIONS", sizeWindow.x / 2 - 100, 25, 200, 75);

	Engine::GetInstance().render->DrawText("General", sizeWindow.x / 2 - 175 - 30, (int)sizeWindow.y / 3 * 1 - 10, 75, 20);
	Engine::GetInstance().render->DrawText(" Music ", sizeWindow.x / 2 - 175 - 30, (int)sizeWindow.y / 3 * 1.5 - 10, 75, 20);
	Engine::GetInstance().render->DrawText("   Sfx   ", sizeWindow.x / 2 - 175 - 30, (int)sizeWindow.y / 3 * 2 - 10, 75, 20);

	//---Slider Display value---//

	// General
	int generalVolumeValue = (int)(generalSlider->GetValue() * 100);
	std::string generalVolumeValueString = std::to_string(generalVolumeValue);
	const char* generalVolumeValueCharPtr = generalVolumeValueString.c_str();
	size_t lengthGeneralValue = strlen(generalVolumeValueCharPtr);
	if (lengthGeneralValue == 1)
	{
		char paddedStringGeneral[100];
		snprintf(paddedStringGeneral, sizeof(paddedStringGeneral), " %s ", generalVolumeValueCharPtr);
		generalVolumeValueCharPtr = paddedStringGeneral;
	}
	Engine::GetInstance().render->DrawText(generalVolumeValueCharPtr, sizeWindow.x / 2 + 100 + 30, (int)sizeWindow.y / 3 * 1 - 10, 25, 20);

	// Music
	int musicVolumeValue = (int)(musicSlider->GetValue() * 100);
	std::string musicVolumeValueString = std::to_string(musicVolumeValue);
	const char* musicVolumeValueCharPtr = musicVolumeValueString.c_str();
	size_t lengthMusicValue = strlen(musicVolumeValueCharPtr);
	if (lengthMusicValue == 1)
	{
		char paddedStringMusic[100];
		snprintf(paddedStringMusic, sizeof(paddedStringMusic), " %s ", musicVolumeValueCharPtr);
		musicVolumeValueCharPtr = paddedStringMusic;
	}
	Engine::GetInstance().render->DrawText(musicVolumeValueCharPtr, sizeWindow.x / 2 + 100 + 30, (int)sizeWindow.y / 3 * 1.5 - 10, 25, 20);

	// Sfx
	int sfxVolumeValue = (int)(sfxSlider->GetValue() * 100);
	std::string sfxVolumeValueString = std::to_string(sfxVolumeValue);
	const char* sfxVolumeValueCharPtr = sfxVolumeValueString.c_str();
	size_t lengthSfxValue = strlen(sfxVolumeValueCharPtr);
	if (lengthSfxValue == 1)
	{
		char paddedStringSfx[100];
		snprintf(paddedStringSfx, sizeof(paddedStringSfx), " %s ", sfxVolumeValueCharPtr);
		sfxVolumeValueCharPtr = paddedStringSfx;
	}
	Engine::GetInstance().render->DrawText(sfxVolumeValueCharPtr, sizeWindow.x / 2 + 100 + 30, (int)sizeWindow.y / 3 * 2 - 10, 25, 20);

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
			Engine::GetInstance().scene->CreateButtons();
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
