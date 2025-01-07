#include "GameHUD.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"

GameHUD::GameHUD()
{
}

bool GameHUD::Update(float dt)
{
	Render();

	return true;
}

bool GameHUD::Render()
{
	return true;
}

bool GameHUD::CleanUp()
{
	LOG("Cleanup player");
	//memory leaks
	return true;
}
