#include "ScenePause.h"
#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "Window.h"
#include "Log.h"
#include <string>
#include <box2d/box2d.h>

#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "GuiSlider.h"

ScenePause::ScenePause() : Module()
{
	name = "scenePause";
}

ScenePause::~ScenePause()
{
}

bool ScenePause::Awake()
{
	LOG("Loading TitleScene");
	bool ret = true;

	int x = 0, y = 0;
	Engine::GetInstance().window->GetWindowSize(x, y);
	sizeWindow = { (float)x,(float)y };

	fadeRect = { 0, 0, (int)sizeWindow.x, (int)sizeWindow.y };

	// Big Button
	int playWidth = 120, playHeight = 50;
	SDL_Rect btPlayPos = { (int)(sizeWindow.x / 2 - playWidth / 2), (int)((sizeWindow.y / 10) * 4.25 - playHeight / 2) , playWidth,playHeight };
	resumeButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "  Resume  ", btPlayPos, this);

	// Small Buttons
	int settingsWidth = 100, settingsHeight = 25;
	SDL_Rect btSettingsPos = { (int)(sizeWindow.x / 2 - settingsWidth / 2),(int)((sizeWindow.y / 10) * 5.5 - settingsHeight / 2), settingsWidth,settingsHeight };
	settingsButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "  Settings  ", btSettingsPos, this);

	int backToTitleWidth = 100, backToTitleHeight = 25;
	SDL_Rect btbackToTitlePos = { (int)(sizeWindow.x / 2 - backToTitleWidth / 2), (int)((sizeWindow.y / 10) * 6.5 - backToTitleHeight / 2) , backToTitleWidth,backToTitleHeight };
	backToTitleButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "    Menu    ", btbackToTitlePos, this);

	int exitWidth = 100, exitHeight = 25;
	SDL_Rect btExitPos = { (int)(sizeWindow.x / 2 - exitWidth / 2), (int)((sizeWindow.y / 10) * 7.5 - exitHeight / 2) , exitWidth,exitHeight };
	exitButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "    Exit    ", btExitPos, this);

	return ret;
}

bool ScenePause::Start()
{
	lvl1 = Engine::GetInstance().textures.get()->Load("Assets/Textures/Level1.png");
	lvl2 = Engine::GetInstance().textures.get()->Load("Assets/Textures/Level2.png");
	lvl3 = Engine::GetInstance().textures.get()->Load("Assets/Textures/Level3.png");

	return true;
}

bool ScenePause::PreUpdate()
{
	return true;
}

bool ScenePause::Update(float dt)
{
	bool ret = true;

	if (drawBg)
	{
		// Draw Background
		Engine::GetInstance().render->DrawText("  PAUSE  ", (int)(sizeWindow.x / 2 - 100), 25, 200, 75);
	}

	// Handle button interactions if no button is currently pressed
	if (!resumePressed && !settingsPressed && !backToTitlePressed && !exitPressed)
	{
		ButtonInteraction();
	}
	
	if (resumePressed)
	{
		drawBg = false;
		resumePressed = false;
		Engine::GetInstance().guiManager->DeleteButtons();
		Engine::GetInstance().scene->CreateButtons();
		Engine::GetInstance().ChangeLoopStateWithoutStart(LoopState::GAME);
	}
	else if (settingsPressed)
	{
		settingsPressed = false;
		drawBg = false;
		Engine::GetInstance().guiManager->DeleteButtons();
		Engine::GetInstance().ChangeLoopState(LoopState::SETTINGS);
	}
	else if (backToTitlePressed)
	{
		backToTitlePressed = false;
		drawBg = false;
		Engine::GetInstance().guiManager->DisableButtons();
		Engine::GetInstance().ChangeLoopState(LoopState::TITLE);
	}
	else if (exitPressed)
	{
		ret = false;
	}

	//------Fade------//
	if (fadingIn)
	{
		double elapsedTime = fadeTimer.ReadMSec();
		if (elapsedTime >= fadetime) {
			opacity = 255;
			fadingIn = false;
		}
		else {
			opacity = static_cast<int>((elapsedTime / (fadetime)) * 255);
		}
		Engine::GetInstance().render->DrawRectangle(fadeRect, 0, 0, 0, opacity);
	}
	if (fadingOut)
	{
		double elapsedTime = fadeTimer.ReadMSec();
		if (elapsedTime >= fadetime) {
			opacity = 0;
			fadingOut = false;
		}
		else {
			opacity = 255 - static_cast<int>((elapsedTime / (fadetime)) * 255);
		}
		Engine::GetInstance().render->DrawRectangle(fadeRect, 0, 0, 0, opacity);
	}
	return ret;
}

bool ScenePause::PostUpdate()
{
	return true;
}

bool ScenePause::CleanUp()
{
	Engine::GetInstance().guiManager->DeleteButtons();
	if (lvl1 != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(lvl1);
	}
	if (lvl2 != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(lvl2);
	}
	if (lvl3 != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(lvl3);
	}
	return true;
}

void ScenePause::ButtonInteraction()
{
	// Handle button pressed states
	if (resumeButton->state == GuiControlState::PRESSED)
	{
		resumePressed = true;
		lvlImageTimer.Start();
		FadeOut();
	}
	else if (settingsButton->state == GuiControlState::PRESSED)
	{
		settingsPressed = true;
	}
	else if (backToTitleButton->state == GuiControlState::PRESSED)
	{
		backToTitlePressed = true;
		imageTimer.Start();
		sceneTimer.Start();
		FadeOut();
	}
	else if (exitButton->state == GuiControlState::PRESSED)
	{
		exitPressed = true;
	}
}

void ScenePause::FadeIn()
{
	fadeTimer.Start();
	fadingIn = true;
	fadingOut = false;
}

void ScenePause::FadeOut()
{
	fadeTimer.Start();
	fadingOut = true;
	fadingIn = false;
}

void ScenePause::ResetFadeStates()
{
	lvlImage_fadeOut = false;
	first_fadeIn = false;
	first_fadeOut = false;
	fadingIn = false;
	fadingOut = false;
	last_fadeIn = false;
	opacity = 255;
}
