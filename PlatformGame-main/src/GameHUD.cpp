#include "GameHUD.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Boss.h"
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
	dieScreen = Engine::GetInstance().textures.get()->Load("Assets/Textures/Die-screen.png");
	lvl2Screen = Engine::GetInstance().textures.get()->Load("Assets/Textures/Level2.png");
	lvl3Screen = Engine::GetInstance().textures.get()->Load("Assets/Textures/Level3.png");

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
	if (Engine::GetInstance().scene->goingToLvl1 && !begin_fadeOut) {
		FadeOut();
		begin_fadeOut = true;
	}
	if (fadeTimer.ReadMSec() > fadetime && Engine::GetInstance().scene->goingToLvl1 && begin_fadeOut) {
		Engine::GetInstance().scene->goingToLvl1 = false;
		ResetFadeStates();
	}

	if (advanceTimer.ReadMSec() > fadetime && advanceTimer.ReadMSec() < advanceTime - fadetime &&
		(Engine::GetInstance().scene->goingToLvl2 || Engine::GetInstance().scene->goingToLvl3) && advance_fadeIn1 && !advance_fadeOut2) {
		if (Engine::GetInstance().scene->goingToLvl2) {
			Engine::GetInstance().render->DrawTexture(lvl2Screen, -Engine::GetInstance().render->camera.x, Engine::GetInstance().render->camera.y);
		}
		else if (Engine::GetInstance().scene->goingToLvl3){
			Engine::GetInstance().render->DrawTexture(lvl3Screen, -Engine::GetInstance().render->camera.x, Engine::GetInstance().render->camera.y);
		}
	}
	if ((Engine::GetInstance().scene->goingToLvl2 || Engine::GetInstance().scene->goingToLvl3) && !advance_fadeIn1) {
		FadeIn();
		advanceTimer.Start();
		advance_fadeIn1 = true;
	}
	if (advanceTimer.ReadMSec() > fadetime && (Engine::GetInstance().scene->goingToLvl2 || Engine::GetInstance().scene->goingToLvl3) &&
		!advance_fadeOut1 && advance_fadeIn1) {
		FadeOut();
		advance_fadeOut1 = true;
	}
	if (advanceTimer.ReadMSec() > advanceTime - (2 * fadetime) && (Engine::GetInstance().scene->goingToLvl2 || Engine::GetInstance().scene->goingToLvl3) &&
		!advance_fadeIn2 && advance_fadeIn1 && advance_fadeOut1) {
		FadeIn();
		advance_fadeIn2 = true;
	}
	if (advanceTimer.ReadMSec() > advanceTime - fadetime && (Engine::GetInstance().scene->goingToLvl2 || Engine::GetInstance().scene->goingToLvl3) &&
		!advance_fadeOut2 && advance_fadeIn1 && advance_fadeOut1 && advance_fadeIn2) {
		FadeOut();
		advance_fadeOut2 = true;
	}
	if (advanceTimer.ReadMSec() > advanceTime && (Engine::GetInstance().scene->goingToLvl2 || Engine::GetInstance().scene->goingToLvl3)) {
		Engine::GetInstance().scene->goingToLvl2 = false;
		Engine::GetInstance().scene->goingToLvl3 = false;
		ResetFadeStates();
	}


	if (!Engine::GetInstance().scene->GetPlayer()->InsideDeadTime() && !Engine::GetInstance().scene->goingToLvl1 && !Engine::GetInstance().scene->goingToLvl2 && !Engine::GetInstance().scene->goingToLvl3) {
		internalDt = dt;

		if (Engine::GetInstance().scene->bossMusicPlayed && Engine::GetInstance().scene->GetCurrentLevel() == 3 && Engine::GetInstance().scene->GetPlayerPosition().getX() >= 5760) {
			Engine::GetInstance().render->DrawRectangle({ -Engine::GetInstance().render->camera.x + 854 / 2 - (10 * Engine::GetInstance().scene->GetBoss()->GetLives())/2, 40, 10 * Engine::GetInstance().scene->GetBoss()->GetLives(), 16}, 100, 0, 0);
		}

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
		ResetFadeStates();
	}
	else if (Engine::GetInstance().scene->GetPlayer()->InsideDeadTime()){
		fadeRect.x = -Engine::GetInstance().render->camera.x;
		fadeRect.y = Engine::GetInstance().render->camera.y;

		if (sceneTimer.ReadMSec() < fadetime) {
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
		}
		if (!first_fadeIn) {
			FadeIn();
			sceneTimer.Start();
			first_fadeIn = true;
		}
		if (sceneTimer.ReadMSec() > fadetime && !first_fadeOut) {
			FadeOut();
			first_fadeOut = true;
		}
		if (sceneTimer.ReadMSec() > fadetime && sceneTimer.ReadMSec() < (sceneTime - fadetime)) {
			Engine::GetInstance().render->DrawTexture(dieScreen, -Engine::GetInstance().render->camera.x, Engine::GetInstance().render->camera.y);
		}
		if (sceneTimer.ReadMSec() > (sceneTime - (fadetime * 2)) && !last_fadeIn) {
			FadeIn();
			last_fadeIn = true;
		}
		if (sceneTimer.ReadMSec() > (sceneTime - fadetime) && !last_fadeOut) {
			FadeOut();
			last_fadeOut = true;
		}
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
	return true;
}

bool GameHUD::CleanUp()
{
	LOG("Cleanup Game HUD");
	Engine::GetInstance().textures->UnLoad(keysMenuTexture);
	Engine::GetInstance().textures->UnLoad(lifeHudTexture);
	Engine::GetInstance().textures->UnLoad(ignisHudTexture);
	Engine::GetInstance().textures->UnLoad(ignisTexture);
	Engine::GetInstance().textures->UnLoad(heartTexture);
	Engine::GetInstance().textures->UnLoad(shurikenCDTexture);
	Engine::GetInstance().textures->UnLoad(attackCDTexture);
	Engine::GetInstance().textures->UnLoad(dieScreen);
	Engine::GetInstance().textures->UnLoad(lvl2Screen);
	Engine::GetInstance().textures->UnLoad(lvl3Screen);

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

void GameHUD::FadeIn()
{
	opacity = 0;
	fadeTimer.Start();
	fadingIn = true;
	fadingOut = false;
}

void GameHUD::FadeOut()
{
	opacity = 255;
	fadeTimer.Start();
	fadingOut = true;
	fadingIn = false;
}

void GameHUD::ResetFadeStates()
{
	begin_fadeOut = false;
	advance_fadeIn1 = false;
	advance_fadeOut1 = false;
	advance_fadeIn2 = false;
	advance_fadeOut2 = false;

	first_fadeIn = false;
	first_fadeOut = false;
	last_fadeIn = false;
	last_fadeOut = false;
	fadingIn = false;
	fadingOut = false;
	opacity = 0;
}
