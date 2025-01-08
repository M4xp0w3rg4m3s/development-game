#include "GameHUD.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include <iomanip>
#include <sstream>

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
	heartTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/heart.png");

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

	heartAnimator = new Sprite(heartTexture);
	heartAnimator->SetNumberAnimations(1);
	for (int i = 0; i < heart_frameNumY; i++) {
		for (int j = 0; j < heart_frameNumX; j++) {
			heartAnimator->AddKeyFrame(0, { j * heart_texW, i * heart_texH, heart_texW, heart_texH });
		}
	}
	heartAnimator->SetAnimationDelay(0, 100);
	heartAnimator->SetAnimation(0);
	heartAnimator->SetLoop(true);

	StartInternalTimer();
	return true;
}

bool GameHUD::Update(float dt)
{
	internalDt = dt;

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		if (!keysMenuOn) keysMenuOn = true;
		else keysMenuOn = false;
	}

	if (keysMenuOn) {
		Engine::GetInstance().render->DrawTexture(keysMenuTexture, -Engine::GetInstance().render->camera.x, Engine::GetInstance().render->camera.y);
	}

	Engine::GetInstance().render->DrawTexture(lifeHudTexture, -Engine::GetInstance().render->camera.x + 16, Engine::GetInstance().render->camera.y + 400);
	Engine::GetInstance().render->DrawRectangle({ -Engine::GetInstance().render->camera.x + 16 + 54, Engine::GetInstance().render->camera.y + 400 + 42, 94 * Engine::GetInstance().scene->GetPlayer()->GetPlayerLives(), 16 }, 100, 0, 0);
	heartAnimator->Update();
	heartAnimator->Draw(-Engine::GetInstance().render->camera.x + 16 + 9, Engine::GetInstance().render->camera.y + 400 + 23, 0, 0);


	Engine::GetInstance().render->DrawTexture(ignisHudTexture, -Engine::GetInstance().render->camera.x + 719, Engine::GetInstance().render->camera.y + 400);
	ignisAnimator->Update();
	ignisAnimator->Draw(-Engine::GetInstance().render->camera.x + 854 - 16 - ignis_texW, Engine::GetInstance().render->camera.y + 400 + 23, 0, 0);
	Engine::GetInstance().render->DrawText(std::to_string(Engine::GetInstance().scene->GetPlayer()->GetPlayerIgnis()).c_str(), 735, 440, 10 * (int)(std::to_string(Engine::GetInstance().scene->GetPlayer()->GetPlayerIgnis()).size()), 18);

	UpdateInternalTimer();
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(1) << internalTimer;
	Engine::GetInstance().render->DrawText(ReadInternalTimerFormat().c_str(), 854 / 2 - 5 * (int)(ReadInternalTimerFormat().size()), 10, 10 * (int)(ReadInternalTimerFormat().size()), 18);
	
	return true;
}

bool GameHUD::CleanUp()
{
	LOG("Cleanup Game HUD");
	Engine::GetInstance().textures->UnLoad(keysMenuTexture);
	Engine::GetInstance().textures->UnLoad(lifeHudTexture);
	Engine::GetInstance().textures->UnLoad(ignisHudTexture);
	delete ignisAnimator;
	delete heartAnimator;
	Engine::GetInstance().textures->UnLoad(ignisTexture);
	Engine::GetInstance().textures->UnLoad(heartTexture);
	//memory leaks
	return true;
}

void GameHUD::StartInternalTimer()
{
	internalTimer = 0;
}

void GameHUD::UpdateInternalTimer()
{
	internalTimer += internalDt/1000;
}

double GameHUD::ReadInternalTimerSec() const
{
	return internalTimer;
}

std::string GameHUD::ReadInternalTimerFormat() const
{
	int minutes = static_cast<int>(ReadInternalTimerSec() / 60);
	int seconds = static_cast<int>(ReadInternalTimerSec()) % 60;

	std::ostringstream stream;
	stream << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;

	return stream.str();
}
