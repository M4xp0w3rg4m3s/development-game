#include "SceneEnd.h"
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

SceneEnd::SceneEnd() : Module()
{
	name = "sceneIntro";
}

// Destructor
SceneEnd::~SceneEnd()
{
}

// Called before render is available
bool SceneEnd::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool SceneEnd::Start()
{
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/End_Screen.wav");

	parallax = Engine::GetInstance().parallax.get();

	parallax->textureName1 = "Assets/Textures/End_para/1.png";
	parallax->textureName2 = "Assets/Textures/End_para/2.png";
	parallax->textureName3 = "Assets/Textures/End_para/3.png";
	parallax->textureName4 = "Assets/Textures/End_para/4.png";
	parallax->textureName5 = "Assets/Textures/End_para/4.png";

	previousWidth = parallax->sizeW;
	parallax->sizeW = 854;

	parallax->ChangeTextures();

	theEnd = Engine::GetInstance().textures.get()->Load("Assets/Textures/End_game.png");

	sceneTimer.Start();

	FadeOut();

	return true;
}
// Called each loop iteration
bool SceneEnd::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool SceneEnd::Update(float dt)
{
	auto mousePosition = Engine::GetInstance().input.get()->GetMousePosition();
	auto& camera = Engine::GetInstance().render->camera;
	static int lastMouseX = (int)mousePosition.getX();
	int deltaX = (int)mousePosition.getX() - lastMouseX;
	camera.x -= deltaX;

	lastMouseX = (int)mousePosition.getX();
	parallax->Update(dt);

	Engine::GetInstance().render->DrawTexture(theEnd, -Engine::GetInstance().render->camera.x, 0);

	if (sceneTimer.ReadMSec() >= (sceneTime - fadetime) && !last_fadeIn) {
		FadeIn();
		last_fadeIn = true;
	}
	else if(sceneTimer.ReadMSec() >= sceneTime) {
		Engine::GetInstance().ChangeLoopState(LoopState::TITLE);
		parallax->sizeW = previousWidth;
	}

	fadeRect.x = -Engine::GetInstance().render->camera.x;

	if (fadingIn) {
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

	if (fadingOut) {
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

	if (sceneTimer.ReadMSec() > sceneTime) {
		Engine::GetInstance().ChangeLoopState(LoopState::TITLE);
	}

	return true;
}

// Called each loop iteration
bool SceneEnd::PostUpdate()
{
	bool ret = true;

	// Detects if the player wants to exit the game with ESC key
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool SceneEnd::CleanUp()
{
	LOG("Freeing Intro Scene");
	parallax = nullptr;
	return true;
}

void SceneEnd::FadeIn()
{
	fadeTimer.Start();
	fadingIn = true;
	fadingOut = false;
}

void SceneEnd::FadeOut()
{
	fadeTimer.Start();
	fadingOut = true;
	fadingIn = false;
}
