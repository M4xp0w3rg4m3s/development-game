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

bool GameHUD::Awake()
{
	return true;
}

bool GameHUD::Start()
{
	LOG("Game Hud start");

	keysMenuTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/controls.png");
	lifeHudTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/life.png");
	ignisTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/ignis.png");
	ignisHudTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/ignis_counter.png");

	ignisAnimator = new Sprite(ignisTexture);
	ignisAnimator->SetNumberAnimations(1);

	for (int i = 0; i < ignis_frameNumY; i++) {
		for (int j = 0; j < ignis_frameNumX; j++) {
			ignisAnimator->AddKeyFrame(0, { j * ignis_texW, i * ignis_texH, ignis_texW, ignis_texH });
		}
	}
	ignisAnimator->SetAnimationDelay(0, 100);
	ignisAnimator->SetAnimation(0);
	ignisAnimator->SetLoop(true);

	return true;
}

bool GameHUD::Update(float dt)
{
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		if (!keysMenuOn) keysMenuOn = true;
		else keysMenuOn = false;
	}

	if (keysMenuOn) {
		Engine::GetInstance().render->DrawTexture(keysMenuTexture, -Engine::GetInstance().render->camera.x, Engine::GetInstance().render->camera.y);
	}

	Engine::GetInstance().render->DrawTexture(lifeHudTexture, -Engine::GetInstance().render->camera.x + 16, Engine::GetInstance().render->camera.y + 400);
	Engine::GetInstance().render->DrawRectangle({ -Engine::GetInstance().render->camera.x + 16 + 54, Engine::GetInstance().render->camera.y + 400 + 42, 94 * Engine::GetInstance().scene->GetPlayer()->GetPlayerLives(), 16 }, 100, 0, 0);

	Engine::GetInstance().render->DrawTexture(ignisHudTexture, -Engine::GetInstance().render->camera.x + 719, Engine::GetInstance().render->camera.y + 400);
	ignisAnimator->Update();
	ignisAnimator->Draw(-Engine::GetInstance().render->camera.x + 854 - 16 - ignis_texW, Engine::GetInstance().render->camera.y + 400 + 23, 0, 0);

	Engine::GetInstance().render->DrawText(std::to_string(Engine::GetInstance().scene->GetPlayer()->GetPlayerIgnis()).c_str(), 735, 440, 10 * std::to_string(Engine::GetInstance().scene->GetPlayer()->GetPlayerIgnis()).size(), 18);

	return true;
}

bool GameHUD::CleanUp()
{
	LOG("Cleanup Game HUD");
	SDL_DestroyTexture(keysMenuTexture);
	SDL_DestroyTexture(lifeHudTexture);
	SDL_DestroyTexture(ignisHudTexture);
	delete ignisAnimator;
	//memory leaks
	return true;
}
