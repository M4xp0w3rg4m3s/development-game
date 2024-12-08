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
#include "Octopus.h"
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
		else if (name == "octopus") {
			Octopus* enemy = (Octopus*)Engine::GetInstance().entityManager->CreateEntity(EntityType::OCTOPUS);
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

	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");

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

	if (player->position.getX() > Engine::GetInstance().window.get()->width / 2) {
		Engine::GetInstance().render.get()->camera.x = -((player->position.getX() + player->width / 2) - (Engine::GetInstance().window.get()->width) / 2);
	}
	else {
		Engine::GetInstance().render.get()->camera.x = 0;
	}

	if (current_level == 1) {
		Engine::GetInstance().render->DrawTexture(caveBg, 0, 0);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {

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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
	
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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		player->ResetPlayer(current_level);
	}

	//Get mouse position and obtain the map coordinate
	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap((int)mousePos.getX() - Engine::GetInstance().render.get()->camera.x,(int)mousePos.getY() - Engine::GetInstance().render.get()->camera.y);

	Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld((int)mouseTile.getX(), (int)mouseTile.getY());
	SDL_Rect rect = { 0,0,32,32 };

	// saves the tile pos for debugging purposes
	if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
		tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
		once = true;
	}

	if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN) {
		if (enemyList[enemyIndex]->active) {
			enemyList[enemyIndex]->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
			enemyList[enemyIndex]->ResetPath();
		}
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		enemyIndex++;
		if (enemyIndex >= numEnemies) {
			enemyIndex = 0;
		}
	}
	
	for (auto checkpoint : Engine::GetInstance().map->checkpoints) {
		if (current_level == 1) {
			if (player->position.getX() > checkpoint->pos.getX() && checkpoint->level == 1 && !checkpoint->activated) {
				checkpoint->activated = true;
				SaveState();
			}
		}
		else if (current_level == 2) {
			if (player->position.getX() > checkpoint->pos.getX() && checkpoint->level == 2 && !checkpoint->activated) {
				checkpoint->activated = true;
				SaveState();
			}
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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		LoadState();

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveState();

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
Vector2D Scene::GetPlayerPosition() const
{
	return player->GetPosition();
}

Player* Scene::GetPlayer() const
{
	return player;
}

int Scene::GetCurrentLevel() const
{
	return current_level;
}

void Scene::LoadState()
{
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Read XML and restore information

	if (sceneNode.child("level").attribute("currentlevel").as_int() == 2 && current_level == 1) {
		Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");

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

	} else if (sceneNode.child("level").attribute("currentlevel").as_int() == 1 && current_level != 1) {
		Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");

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

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("player").attribute("x").as_float(), sceneNode.child("player").attribute("y").as_float());
	player->SetPosition(playerPos);

	//enemies
	for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Vector2D enemyPos = Vector2D(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());

		bool enemyActive = enemyNode.attribute("active").as_bool();

		for (const auto& entity : Engine::GetInstance().entityManager->entities) {
			if (entity->type == EntityType::BEE || entity->type == EntityType::BOAR || entity->type == EntityType::OCTOPUS || entity->type == EntityType::HEDGEHOG) {

				Enemy* enemy = static_cast<Enemy*>(entity);

				if (enemy && enemy->enemyId == enemyNode.attribute("id").as_string()) {
					if (!enemy->active && enemyActive) {
						enemy->Enable();
					}
					if (enemyActive) {
						enemy->SetPosition(enemyPos);
					}
				}
			}
		}
	}
}

void Scene::SaveState()
{
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Save info to XML 

	//Player position
	sceneNode.child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("player").attribute("y").set_value(player->GetPosition().getY());

	sceneNode.child("level").attribute("currentlevel").set_value(current_level);

	//enemies
	for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		bool enemyActive = enemyNode.attribute("active").as_bool();

		for (const auto& entity : Engine::GetInstance().entityManager->entities) {
			if (entity->type == EntityType::BEE || entity->type == EntityType::BOAR || entity->type == EntityType::OCTOPUS || entity->type == EntityType::HEDGEHOG) {

				Enemy* enemy = static_cast<Enemy*>(entity);

				if (enemy && enemy->enemyId == enemyNode.attribute("id").as_string()) {
					if (enemyActive) {
						if (!enemy->active) {
							enemyNode.attribute("active").set_value("false");
						}
						else {
							enemyNode.attribute("x").set_value(enemy->GetPosition().getX());
							enemyNode.attribute("y").set_value(enemy->GetPosition().getY());
						}
					}
				}
			}
		}
	}

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}
