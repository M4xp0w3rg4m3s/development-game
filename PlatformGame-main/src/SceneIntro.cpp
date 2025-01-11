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
	if (imageTimer.ReadMSec() < image1Time * 1000) {
		Engine::GetInstance().render->DrawTexture(intro1, 0, 0);
	}
	else {
		Engine::GetInstance().render->DrawTexture(intro2, 0, 0);
	}

	if (sceneTimer.ReadMSec() > sceneTime * 1000) {
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