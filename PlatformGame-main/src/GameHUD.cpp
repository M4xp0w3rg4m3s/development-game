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
	shurikenCDTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/cooldown_shuriken.png");
	attackCDTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/cooldown_attack.png");

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

	shurikenCDAnimator = new Sprite(shurikenCDTexture);
	shurikenCDAnimator->SetNumberAnimations(1);
	for (int i = 0; i < framesCD; i++) {
		shurikenCDAnimator->AddKeyFrame(0, { i * CD_texW, 0, CD_texW, CD_texH });
	}
	shurikenCDAnimator->SetAnimationDelay(0, (int)(Engine::GetInstance().scene->GetPlayer()->GetShurikenTime() * 0.03));
	shurikenCDAnimator->SetAnimation(0);
	shurikenCDAnimator->SetLoop(false);

	attackCDAnimator = new Sprite(attackCDTexture);
	attackCDAnimator->SetNumberAnimations(1);
	for (int i = 0; i < framesCD; i++) {
		attackCDAnimator->AddKeyFrame(0, { i * CD_texW, 0, CD_texW, CD_texH });
	}
	attackCDAnimator->SetAnimationDelay(0, (int)(Engine::GetInstance().scene->GetPlayer()->GetAttackTime() * 0.03));
	attackCDAnimator->SetAnimation(0);
	attackCDAnimator->SetLoop(false);

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

	if (Engine::GetInstance().scene->GetPlayer()->GetAttackTimer() < Engine::GetInstance().scene->GetPlayer()->GetAttackTime() && !beforeAttack) {
		if (!attackInCD) {
			attackCDAnimator->SetAnimation(0);
		}
		attackCDAnimator->Update();
		attackCDAnimator->Draw(-Engine::GetInstance().render->camera.x + 16, Engine::GetInstance().render->camera.y + 400 - 64, 0, 0);
		attackInCD = true;
	}
	if ((Engine::GetInstance().scene->GetPlayer()->GetPlayerState() == PlayerState::WOMBO || Engine::GetInstance().scene->GetPlayer()->GetPlayerState() == PlayerState::COMBO) && !playerAttacked) {
		attackInCD = false;
		beforeAttack = false;
		playerAttacked = true;
	}
	if ((Engine::GetInstance().scene->GetPlayer()->GetPlayerState() != PlayerState::WOMBO || Engine::GetInstance().scene->GetPlayer()->GetPlayerState() != PlayerState::COMBO) && playerAttacked) {
		playerAttacked = false;
	}

	if (Engine::GetInstance().scene->GetPlayer()->GetShurikenTimer() < Engine::GetInstance().scene->GetPlayer()->GetShurikenTime() && Engine::GetInstance().scene->GetPlayer()->IsShurikenEnabled()) {
		if (!shurikenInCD) {
			shurikenCDAnimator->SetAnimation(0);
		}
		shurikenCDAnimator->Update();
		shurikenCDAnimator->Draw(-Engine::GetInstance().render->camera.x + 854 - 16 - CD_texW, Engine::GetInstance().render->camera.y + 400 - 64, 0, 0);
		shurikenInCD = true;
	}
	if (Engine::GetInstance().scene->GetPlayer()->GetShurikenTimer() > Engine::GetInstance().scene->GetPlayer()->GetShurikenTime()) {
		shurikenInCD = false;
	}


	UpdateInternalTimer();
	std::ostringstream stream;
	stream << std::fixed << std::setprecision(1) << internalTimer;
	Engine::GetInstance().render->DrawText(ReadInternalTimerFormat().c_str(), 854 / 2 - 5 * (int)(ReadInternalTimerFormat().size()), 10, 10 * (int)(ReadInternalTimerFormat().size()), 18);
	
	return true;
}

bool GameHUD::CleanUp()
{
	LOG("Cleanup Game HUD");/*
	Engine::GetInstance().textures->UnLoad(keysMenuTexture);
	Engine::GetInstance().textures->UnLoad(lifeHudTexture);
	Engine::GetInstance().textures->UnLoad(ignisHudTexture);
	Engine::GetInstance().textures->UnLoad(ignisTexture);
	Engine::GetInstance().textures->UnLoad(heartTexture);
	Engine::GetInstance().textures->UnLoad(shurikenCDTexture);
	Engine::GetInstance().textures->UnLoad(attackCDTexture);*/
	ignisAnimator->Release();
	heartAnimator->Release();
	shurikenCDAnimator->Release();
	attackCDAnimator->Release();
	delete ignisAnimator;
	delete heartAnimator;
	delete shurikenCDAnimator;
	delete attackCDAnimator;
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
