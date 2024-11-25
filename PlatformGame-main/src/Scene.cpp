#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"
#include "Parallax.h"
#include "Enemy.h"
#include <string>

#include "Boar.h"
#include "Mushroom.h"
#include "Bee.h"
#include "Hedgehog.h"

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;
}

// Destructor
Scene::~Scene()
{
}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	parallax = Engine::GetInstance().parallax.get();

	parallax->textureName1 = configParameters.child("layers").child("one").attribute("texturePath").as_string();
	parallax->textureName2 = configParameters.child("layers").child("two").attribute("texturePath").as_string();
	parallax->textureName3 = configParameters.child("layers").child("three").attribute("texturePath").as_string();
	parallax->textureName4 = configParameters.child("layers").child("four").attribute("texturePath").as_string();
	parallax->textureName5 = configParameters.child("layers").child("five").attribute("texturePath").as_string();

	//Get the map name from the config file and assigns the value
	Engine::GetInstance().map.get()->mapName = configParameters.child("map").attribute("name").as_string();
	Engine::GetInstance().map.get()->mapPath = configParameters.child("map").attribute("path").as_string();

	//Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);

	//Get the player texture name from the config file and assigns the value
	player->textureName = configParameters.child("player").attribute("texturePath").as_string();

	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		std::string name = enemyNode.attribute("name").as_string();
		if (name == "boar") {
			Boar* enemy = (Boar*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOAR);
			enemy->SetParameters(enemyNode);
			enemyList.push_back(enemy);
		}
		else if (name == "mushroom") {
			Mushroom* enemy = (Mushroom*)Engine::GetInstance().entityManager->CreateEntity(EntityType::MUSHROOM);
			enemy->SetParameters(enemyNode);
			enemyList.push_back(enemy);
		}
		else if (name == "bee") {
			Bee* enemy = (Bee*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BEE);
			enemy->SetParameters(enemyNode);
			enemyList.push_back(enemy);
		}
		else if (name == "hedgehog") {
			Hedgehog* enemy = (Hedgehog*)Engine::GetInstance().entityManager->CreateEntity(EntityType::HEDGEHOG);
			enemy->SetParameters(enemyNode);
			enemyList.push_back(enemy);
		}
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	Engine::GetInstance().map->Load("Assets/Maps/", "Level1Map.tmx");

	caveBg = Engine::GetInstance().textures.get()->Load("Assets/Maps/background_final1.png");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (-(player->position.getX() - (Engine::GetInstance().window.get()->width) / 2) < 0) {
		Engine::GetInstance().render.get()->camera.x = -((player->position.getX() + player->width/2) - (Engine::GetInstance().window.get()->width) / 2);
	}

	if (current_level == 1) {
		Engine::GetInstance().render->DrawTexture(caveBg, 0, 0);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {

		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level2Map.tmx", true);

		parallax->textureName1 = configParameters.child("layers2").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers2").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers2").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers2").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers2").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();

		SDL_DestroyTexture(caveBg);

		current_level = 2;

		player->ResetPlayer(current_level);
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {

		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level1Map.tmx",true);

		parallax->textureName1 = configParameters.child("layers").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();

		caveBg = Engine::GetInstance().textures.get()->Load("Assets/Maps/background_final1.png");

		current_level = 1;

		player->ResetPlayer(current_level);
	}

	//Get mouse position and obtain the map coordinate
	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap(mousePos.getX() - Engine::GetInstance().render.get()->camera.x,
																	mousePos.getY() - Engine::GetInstance().render.get()->camera.y);

	Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld(mouseTile.getX(), mouseTile.getY());
	SDL_Rect rect = { 0,0,32,32 };

	// saves the tile pos for debugging purposes
	if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
		tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
		once = true;
	}

	if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN) {
		enemyList[enemyIndex]->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
		enemyList[enemyIndex]->ResetPath();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		if (enemyList.size() > enemyIndex) {
			enemyIndex++;
		}
		else {
			enemyIndex = 0;
		}
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	// Detects if the player wants to exit the game with ESC key
	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	if (current_level = 1) {
		SDL_DestroyTexture(caveBg);
	}
	SDL_DestroyTexture(img);

	return true;
}

void Scene::CameraReset()
{
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;
}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}