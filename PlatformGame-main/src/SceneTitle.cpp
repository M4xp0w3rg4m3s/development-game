#include "SceneTitle.h"
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

SceneTitle::SceneTitle() : Module()
{
	name = "sceneIntro";
}

SceneTitle::~SceneTitle()
{
}

bool SceneTitle::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	int x = 0, y = 0;
	Engine::GetInstance().window->GetWindowSize(x, y);
	sizeWindow = {(float)x,(float)y};

	fadeRect = { 0, 0, (int)sizeWindow.x, (int)sizeWindow.y };

	// Big Button
	int playWidth = 120, playHeight = 50;
	SDL_Rect btPlayPos = { sizeWindow.x / 2 - playWidth/2, (sizeWindow.y / 10) * 4.25 - playHeight/2 , playWidth,playHeight};
	playButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "  Play  ", btPlayPos, this);

	// Small Buttons
	int continueWidth = 100, continueHeight = 25;
	SDL_Rect btContinuePos = { sizeWindow.x / 2 - continueWidth / 2, (sizeWindow.y / 10) * 5.5 - continueHeight / 2 , continueWidth,continueHeight };
	continueButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "  Continue  ", btContinuePos, this);

	int settingsWidth = 100, settingsHeight = 25;
	SDL_Rect btSettingsPos = { sizeWindow.x / 2 - settingsWidth / 2, (sizeWindow.y / 10) * 6.5 - settingsHeight / 2 , settingsWidth,settingsHeight };
	settingsButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "  Settings  ", btSettingsPos, this);

	int creditsWidth = 100, creditsHeight = 25;
	SDL_Rect btCreditsPos = { sizeWindow.x / 2 - creditsWidth / 2, (sizeWindow.y / 10) * 7.5 - creditsHeight / 2 , creditsWidth,creditsHeight };
	creditsButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "   Credits   ", btCreditsPos, this);

	int exitWidth = 100, exitHeight = 25;
	SDL_Rect btExitPos = { sizeWindow.x / 2 - exitWidth / 2, (sizeWindow.y / 10) * 8.5 - exitHeight / 2 , exitWidth,exitHeight };
	exitButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "    Exit    ", btExitPos, this);

	return ret;
}

bool SceneTitle::Start()
{
	//intro music
	//Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");

	credits1 = Engine::GetInstance().textures.get()->Load("Assets/Textures/intro_1.png");
	credits2 = Engine::GetInstance().textures.get()->Load("Assets/Textures/intro_2.png");

	return true;
}

bool SceneTitle::PreUpdate()
{
	return true;
}

bool SceneTitle::Update(float dt)
{
	bool ret = true;

	// Toggle debug mode
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		debug = !debug;
	}

	// Handle button interactions if no button is currently pressed
	if (!playPressed && !continuePressed && !settingsPressed && !creditsPressed && !exitPressed)
	{
		ButtonInteraction();
	}

	if (fadeTimer.ReadMSec() > fadeTime)
	{
		if (playPressed)
		{
			Engine::GetInstance().guiManager->DeleteButtons();
			Engine::GetInstance().ChangeLoopState(LoopState::GAME);
		}
		else if (continuePressed)
		{
			// Continue functionality can be implemented here
		}
		else if (settingsPressed)
		{
			// Settings functionality can be implemented here
		}
		else if (creditsPressed)
		{
			Engine::GetInstance().guiManager->DeleteButtons();
			HandleCredits();
		}
		else if (exitPressed)
		{
			ret = false;
		}
	}

	if (fadingIn)
	{
		UpdateFadeIn();
	}
	if (fadingOut)
	{
		UpdateFadeOut();
	}

	return ret;
}

bool SceneTitle::PostUpdate()
{
	bool ret = true;

	// Detects if the player wants to exit the game with ESC key
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool SceneTitle::CleanUp()
{
	LOG("Freeing Intro Scene");
	if (credits1 != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(credits1);
	}
	if (credits2 != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(credits2);
	}

	return true;
}

void SceneTitle::ButtonInteraction()
{
	// Handle button pressed states
	if (playButton->state == GuiControlState::PRESSED)
	{
		FadeOut();
		playPressed = true;
	}
	else if (continueButton->state == GuiControlState::PRESSED) // && there is a previous game
	{
		FadeOut();
		continuePressed = true;
	}
	else if (settingsButton->state == GuiControlState::PRESSED)
	{
		FadeOut();
		settingsPressed = true;
	}
	else if (creditsButton->state == GuiControlState::PRESSED)
	{
		FadeOut();
		creditsPressed = true;
		imageTimer.Start();
		sceneTimer.Start();
	}
	else if (exitButton->state == GuiControlState::PRESSED)
	{
		FadeOut();
		exitPressed = true;
	}
}
void SceneTitle::UpdateFadeIn()
{
	double elapsedTime = fadeTimer.ReadMSec();
	if (elapsedTime >= fadeTime)
	{
		opacity = 0;
		fadingIn = false;
	}
	else
	{
		opacity = 255 - static_cast<int>((elapsedTime / fadeTime) * 255);
	}
	Engine::GetInstance().render->DrawRectangle(fadeRect, 0, 0, 0, opacity);
}

void SceneTitle::UpdateFadeOut()
{
	double elapsedTime = fadeTimer.ReadMSec();
	if (elapsedTime >= fadeTime)
	{
		opacity = 255;
		fadingOut = false;
	}
	else
	{
		opacity = static_cast<int>((elapsedTime / fadeTime) * 255);
	}
	Engine::GetInstance().render->DrawRectangle(fadeRect, 0, 0, 0, opacity);
}

void SceneTitle::FadeIn()
{
	fadeTimer.Start();
	fadingIn = true;
	fadingOut = false;
	opacity = 255;
}

void SceneTitle::FadeOut()
{
	fadeTimer.Start();
	fadingOut = true;
	fadingIn = false;
	opacity = 0;
}
void SceneTitle::HandleCredits()
{
	double currentTime = imageTimer.ReadMSec();

	if (currentTime < image1Time)
	{
		Engine::GetInstance().render->DrawTexture(credits1, 0, 0);

		if (currentTime >= (image1Time - fadeTime) && !first_fadeOut)
		{
			FadeOut();
			first_fadeOut = true;
		}
		if (currentTime <= fadeTime && !first_fadeIn)
		{
			FadeIn();
			first_fadeIn = true;
		}
	}
	else if (currentTime < (image1Time + image2Time))
	{
		Engine::GetInstance().render->DrawTexture(credits2, 0, 0);

		double secondImageTime = currentTime - image1Time;

		if (secondImageTime <= fadeTime && !second_fadeIn)
		{
			FadeIn();
			second_fadeIn = true;
		}

		if (secondImageTime >= (image2Time - fadeTime) && !second_fadeOut)
		{
			FadeOut();
			second_fadeOut = true;
		}
	}
	else
	{
		creditsPressed = false;
		ResetFadeStates();

		// Big Button
		int playWidth = 120, playHeight = 50;
		SDL_Rect btPlayPos = { sizeWindow.x / 2 - playWidth / 2, (sizeWindow.y / 10) * 4.25 - playHeight / 2 , playWidth,playHeight };
		playButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "  Play  ", btPlayPos, this);

		// Small Buttons
		int continueWidth = 100, continueHeight = 25;
		SDL_Rect btContinuePos = { sizeWindow.x / 2 - continueWidth / 2, (sizeWindow.y / 10) * 5.5 - continueHeight / 2 , continueWidth,continueHeight };
		continueButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "  Continue  ", btContinuePos, this);

		int settingsWidth = 100, settingsHeight = 25;
		SDL_Rect btSettingsPos = { sizeWindow.x / 2 - settingsWidth / 2, (sizeWindow.y / 10) * 6.5 - settingsHeight / 2 , settingsWidth,settingsHeight };
		settingsButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "  Settings  ", btSettingsPos, this);

		int creditsWidth = 100, creditsHeight = 25;
		SDL_Rect btCreditsPos = { sizeWindow.x / 2 - creditsWidth / 2, (sizeWindow.y / 10) * 7.5 - creditsHeight / 2 , creditsWidth,creditsHeight };
		creditsButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "   Credits   ", btCreditsPos, this);

		int exitWidth = 100, exitHeight = 25;
		SDL_Rect btExitPos = { sizeWindow.x / 2 - exitWidth / 2, (sizeWindow.y / 10) * 8.5 - exitHeight / 2 , exitWidth,exitHeight };
		exitButton = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "    Exit    ", btExitPos, this);
	}
}

void SceneTitle::ResetFadeStates()
{
	first_fadeIn = false;
	first_fadeOut = false;
	second_fadeIn = false;
	second_fadeOut = false;
	fadingIn = false;
	fadingOut = false;
	opacity = 0;
}