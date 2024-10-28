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
#include <string>

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;
}

// Destructor
Scene::~Scene()
{
	delete world;
}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	world = new b2World({ 0, 10 });

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

		player->ResetPlayer();
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

		player->ResetPlayer();
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
		SDL_DestroyTexture(img);
	}
	SDL_DestroyTexture(caveBg);

	return true;
}

void Scene::CameraReset()
{
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;
}
