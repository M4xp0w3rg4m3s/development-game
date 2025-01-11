#include "SceneIntro.h"
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

SceneIntro::SceneIntro() : Module()
{
	name = "sceneIntro";
}

// Destructor
SceneIntro::~SceneIntro()
{
}

// Called before render is available
bool SceneIntro::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool SceneIntro::Start()
{
	//intro music
	//Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");

	intro1 = Engine::GetInstance().textures.get()->Load("Assets/Textures/intro_1.png");
	intro2 = Engine::GetInstance().textures.get()->Load("Assets/Textures/intro_2.png");

	imageTimer.Start();
	sceneTimer.Start();

	FadeOut();

	return true;
}
// Called each loop iteration
bool SceneIntro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool SceneIntro::Update(float dt)
{
	
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;
	if (imageTimer.ReadMSec() < image1Time) {
		Engine::GetInstance().render->DrawTexture(intro1, 0, 0);
		if (imageTimer.ReadMSec() >= (image1Time - fadetime) && !first_fadeIn) {
			FadeIn();
			first_fadeIn = true;
		}
	}
	else {
		if (!first_fadeOut) {
			FadeOut();
			first_fadeOut = true;
		}
		if (sceneTimer.ReadMSec() >= (sceneTime - fadetime) && !last_fadeIn) {
			FadeIn();
			last_fadeIn = true;
		}
		Engine::GetInstance().render->DrawTexture(intro2, 0, 0);
	}

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
		Engine::GetInstance().ChangeLoopState(LoopState::GAME);
	}

	return true;
}

// Called each loop iteration
bool SceneIntro::PostUpdate()
{
	bool ret = true;

	// Detects if the player wants to exit the game with ESC key
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool SceneIntro::CleanUp()
{
	LOG("Freeing Intro Scene");
	if (intro1 != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(intro1);
	}
	if (intro2 != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(intro2);
	}
	return true;
}

void SceneIntro::FadeIn()
{
	fadeTimer.Start();
	fadingIn = true;
	fadingOut = false;
}

void SceneIntro::FadeOut()
{
	fadeTimer.Start();
	fadingOut = true;
	fadingIn = false;
}
