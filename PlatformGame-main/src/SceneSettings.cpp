#include "SceneSettings.h"
#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "ScenePause.h"
#include "SceneTitle.h"
#include "Window.h"
#include "Log.h"
#include <string>

#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "GuiSlider.h"
#include "GuiToggle.h"
#include "Parallax.h"

SceneSettings::SceneSettings() : Module()
{
	name = "sceneSettings";
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

	fullscreen = (GuiToggle*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::TOGGLE, 6, "  Fullscreen  ", { (int)sizeWindow.x / 2 - 100 / 2, (int)(sizeWindow.y / 3 * 2.5), 100, 20 }, this);

	parallax = Engine::GetInstance().parallax.get();

	parallax->textureName1 = "Assets/Textures/Settings_para/1.png";
	parallax->textureName2 = "Assets/Textures/Settings_para/2.png";
	parallax->textureName3 = "Assets/Textures/Settings_para/3.png";
	parallax->textureName4 = "Assets/Textures/Settings_para/4.png";
	parallax->textureName5 = "Assets/Textures/Settings_para/4.png";

	parallax->ChangeTextures();

	drawBg = true;

	return ret;
}

bool SceneSettings::Start()
{
	return true;
}

bool SceneSettings::PreUpdate()
{
	return true;
}

bool SceneSettings::Update(float dt)
{
	bool ret = true;

	if (drawBg)
	{
		auto mousePosition = Engine::GetInstance().input.get()->GetMousePosition();
		auto& camera = Engine::GetInstance().render->camera;
		static int lastMouseX = (int)mousePosition.getX();
		int deltaX = (int)mousePosition.getX() - lastMouseX;
		camera.x -= deltaX;
		lastMouseX = (int)mousePosition.getX();
		parallax->Update(dt);

	}
	else {
		Engine::GetInstance().render->camera.x = 0;
		Engine::GetInstance().render->camera.y = 0;
	}

	//---Text---//
	Engine::GetInstance().render->DrawText("SETTINGS", (int)(sizeWindow.x / 2 - 100), 25, 200, 75);

	Engine::GetInstance().render->DrawText("General", (int)(sizeWindow.x / 2 - 175 - 30), (int)(sizeWindow.y / 3 * 1 - 10), 75, 20);
	Engine::GetInstance().render->DrawText(" Music ", (int)(sizeWindow.x / 2 - 175 - 30), (int)(sizeWindow.y / 3 * 1.5 - 10), 75, 20);
	Engine::GetInstance().render->DrawText("   Sfx   ", (int)(sizeWindow.x / 2 - 175 - 30), (int)(sizeWindow.y / 3 * 2 - 10), 75, 20);

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
	Engine::GetInstance().render->DrawText(generalVolumeValueCharPtr, (int)(sizeWindow.x / 2 + 100 + 30), (int)sizeWindow.y / 3 * 1 - 10, 25, 20);

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
	Engine::GetInstance().render->DrawText(musicVolumeValueCharPtr, (int)(sizeWindow.x / 2 + 100 + 30), (int)(sizeWindow.y / 3 * 1.5 - 10), 25, 20);

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
	Engine::GetInstance().render->DrawText(sfxVolumeValueCharPtr, (int)(sizeWindow.x / 2 + 100 + 30), (int)sizeWindow.y / 3 * 2 - 10, 25, 20);

	if (fullscreen->state == GuiControlState::NORMAL && pressed_once) {
		if(!FirstTime)
		{
			Uint32 flags = SDL_GetWindowFlags(Engine::GetInstance().window->window);

			if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
				SDL_SetWindowFullscreen(Engine::GetInstance().window->window, 0);
			}
			else {
				SDL_SetWindowFullscreen(Engine::GetInstance().window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
		}
		else
		{
			FirstTime = false;
		}
		pressed_once = false;
		once = false;
	}
	else if (fullscreen->state == GuiControlState::PRESSED && !once)
	{
		if (!FirstTime)
		{
			Uint32 flags = SDL_GetWindowFlags(Engine::GetInstance().window->window);

			if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
				SDL_SetWindowFullscreen(Engine::GetInstance().window->window, 0);
			}
			else {
				SDL_SetWindowFullscreen(Engine::GetInstance().window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			}
		}
		else
		{
			FirstTime = false;
		}
		pressed_once = true;
		once = true;
	}
	
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
		drawBg = false;
		exitButtonPressed = false;
		Engine::GetInstance().guiManager->DeleteButtons();
		if (Engine::GetInstance().previousLoopState == LoopState::PAUSE)
		{
			Engine::GetInstance().scene->CreateButtons();
			Engine::GetInstance().ChangeLoopState(Engine::GetInstance().previousLoopState);
			Engine::GetInstance().scenePause->parallax->textureName1 = "Assets/Textures/Pause_para/1.png";
			Engine::GetInstance().scenePause->parallax->textureName2 = "Assets/Textures/Pause_para/2.png";
			Engine::GetInstance().scenePause->parallax->textureName3 = "Assets/Textures/Pause_para/3.png";
			Engine::GetInstance().scenePause->parallax->textureName4 = "Assets/Textures/Pause_para/4.png";
			Engine::GetInstance().scenePause->parallax->textureName5 = "Assets/Textures/Pause_para/4.png";
			Engine::GetInstance().scenePause->parallax->ChangeTextures();
		}
		else if (Engine::GetInstance().previousLoopState == LoopState::TITLE)
		{
			Engine::GetInstance().ChangeLoopState(Engine::GetInstance().previousLoopState);
			Engine::GetInstance().sceneTitle->parallax->textureName1 = "Assets/Textures/Title_para/1.png";
			Engine::GetInstance().sceneTitle->parallax->textureName2 = "Assets/Textures/Title_para/2.png";
			Engine::GetInstance().sceneTitle->parallax->textureName3 = "Assets/Textures/Title_para/3.png";
			Engine::GetInstance().sceneTitle->parallax->textureName4 = "Assets/Textures/Title_para/4.png";
			Engine::GetInstance().sceneTitle->parallax->textureName5 = "Assets/Textures/Title_para/4.png";
			Engine::GetInstance().sceneTitle->parallax->ChangeTextures();
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
