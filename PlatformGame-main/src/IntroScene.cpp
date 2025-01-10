#include "IntroScene.h"

#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include "Sprite.h"
#include "Timer.h"

IntroScene::IntroScene() : Module()
{
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Items/goldCoin.png");
}

IntroScene::~IntroScene()
{
}

bool IntroScene::Awake()
{
	return true;
}

bool IntroScene::Start()
{
	introTimer.Start();

	return true;
}

bool IntroScene::Update(float dt)
{
	if (introTimer.ReadSec() > introTime)
	{
		
	}
	Engine::GetInstance().render.get()->DrawTexture(texture, 0, 0);

	return true;
}

bool IntroScene::CleanUp()
{
	Engine::GetInstance().textures->UnLoad(texture);

	return true;
}
