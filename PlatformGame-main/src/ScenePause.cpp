#include "ScenePause.h"
#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "SceneSettings.h"
#include "SceneTitle.h"
#include "Window.h"
#include "Log.h"
#include <string>
#include <box2d/box2d.h>

#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "GuiSlider.h"
#include "Parallax.h"

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

	parallax = Engine::GetInstance().parallax.get();

	parallax->textureName1 = "Assets/Textures/Pause_para/1.png";
	parallax->textureName2 = "Assets/Textures/Pause_para/2.png";
	parallax->textureName3 = "Assets/Textures/Pause_para/3.png";
	parallax->textureName4 = "Assets/Textures/Pause_para/4.png";
	parallax->textureName5 = "Assets/Textures/Pause_para/4.png";

	parallax->ChangeTextures();

	drawBg = true;

	return ret;
}

bool ScenePause::Start()
{
	
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
		auto mousePosition = Engine::GetInstance().input.get()->GetMousePosition();
		auto& camera = Engine::GetInstance().render->camera;
		static int lastMouseX = (int)mousePosition.getX();
		int deltaX = (int)mousePosition.getX() - lastMouseX;
		camera.x -= deltaX;
		lastMouseX = (int)mousePosition.getX();
		parallax->Update(dt);

		Engine::GetInstance().render->DrawText("  PAUSED  ", (int)(sizeWindow.x / 2 - 100), 25, 200, 75);
	}
	else {
		Engine::GetInstance().render->camera.x = 0;
		Engine::GetInstance().render->camera.y = 0;
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

		if (Engine::GetInstance().scene->GetCurrentLevel() == 1) {
			parallax->textureName1 = Engine::GetInstance().scene->configParameters.child("layers").child("one").attribute("texturePath").as_string();
			parallax->textureName2 = Engine::GetInstance().scene->configParameters.child("layers").child("two").attribute("texturePath").as_string();
			parallax->textureName3 = Engine::GetInstance().scene->configParameters.child("layers").child("three").attribute("texturePath").as_string();
			parallax->textureName4 = Engine::GetInstance().scene->configParameters.child("layers").child("four").attribute("texturePath").as_string();
			parallax->textureName5 = Engine::GetInstance().scene->configParameters.child("layers").child("five").attribute("texturePath").as_string();
		}
		else if (Engine::GetInstance().scene->GetCurrentLevel() == 2) {
			parallax->textureName1 = Engine::GetInstance().scene->configParameters.child("layers2").child("one").attribute("texturePath").as_string();
			parallax->textureName2 = Engine::GetInstance().scene->configParameters.child("layers2").child("two").attribute("texturePath").as_string();
			parallax->textureName3 = Engine::GetInstance().scene->configParameters.child("layers2").child("three").attribute("texturePath").as_string();
			parallax->textureName4 = Engine::GetInstance().scene->configParameters.child("layers2").child("four").attribute("texturePath").as_string();
			parallax->textureName5 = Engine::GetInstance().scene->configParameters.child("layers2").child("five").attribute("texturePath").as_string();
		}
		else if (Engine::GetInstance().scene->GetCurrentLevel() == 3) {
			parallax->textureName1 = Engine::GetInstance().scene->configParameters.child("layers3").child("one").attribute("texturePath").as_string();
			parallax->textureName2 = Engine::GetInstance().scene->configParameters.child("layers3").child("two").attribute("texturePath").as_string();
			parallax->textureName3 = Engine::GetInstance().scene->configParameters.child("layers3").child("three").attribute("texturePath").as_string();
			parallax->textureName4 = Engine::GetInstance().scene->configParameters.child("layers3").child("four").attribute("texturePath").as_string();
			parallax->textureName5 = Engine::GetInstance().scene->configParameters.child("layers3").child("five").attribute("texturePath").as_string();
		}
		parallax->ChangeTextures();
	}
	else if (settingsPressed)
	{
		settingsPressed = false;
		drawBg = false;
		Engine::GetInstance().guiManager->DeleteButtons();
		Engine::GetInstance().ChangeLoopState(LoopState::SETTINGS);
		Engine::GetInstance().sceneSettings->parallax->textureName1 = "Assets/Textures/Settings_para/1.png";
		Engine::GetInstance().sceneSettings->parallax->textureName2 = "Assets/Textures/Settings_para/2.png";
		Engine::GetInstance().sceneSettings->parallax->textureName3 = "Assets/Textures/Settings_para/3.png";
		Engine::GetInstance().sceneSettings->parallax->textureName4 = "Assets/Textures/Settings_para/4.png";
		Engine::GetInstance().sceneSettings->parallax->textureName5 = "Assets/Textures/Settings_para/4.png";
		Engine::GetInstance().sceneSettings->parallax->ChangeTextures();
	}
	else if (backToTitlePressed)
	{
		backToTitlePressed = false;
		drawBg = false;
		Engine::GetInstance().guiManager->DisableButtons();
		Engine::GetInstance().ChangeLoopState(LoopState::TITLE);
		Engine::GetInstance().sceneTitle->parallax->textureName1 = "Assets/Textures/Title_para/1.png";
		Engine::GetInstance().sceneTitle->parallax->textureName2 = "Assets/Textures/Title_para/2.png";
		Engine::GetInstance().sceneTitle->parallax->textureName3 = "Assets/Textures/Title_para/3.png";
		Engine::GetInstance().sceneTitle->parallax->textureName4 = "Assets/Textures/Title_para/4.png";
		Engine::GetInstance().sceneTitle->parallax->textureName5 = "Assets/Textures/Title_para/4.png";
		Engine::GetInstance().sceneTitle->parallax->ChangeTextures();
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
