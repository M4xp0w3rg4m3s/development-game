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
#include "Enemy.h"

#include "GuiControl.h"
#include "GuiManager.h"

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

	SDL_Rect btPos = { 854-65, 0, 65, 20 };
	guiBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, " Options ", btPos, this);

	//Get the map name from the config file and assigns the value
	Engine::GetInstance().map.get()->mapName = configParameters.child("map").attribute("name").as_string();
	Engine::GetInstance().map.get()->mapPath = configParameters.child("map").attribute("path").as_string();

	//Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);

	//Get the player texture name from the config file and assigns the value
	player->textureName = configParameters.child("player").attribute("texturePath").as_string();

	CreateEnemies(configParameters.child("entities").child("enemies_lvl_1").child("enemy"), enemyListLevel1);
	CreateItems(configParameters.child("entities").child("items_lvl_1").child("item"), itemListLevel1);

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

		for (const auto& enemy : enemyListLevel1) {
			enemy->Disable();
		}
		for (const auto& item : itemListLevel1) {
			item->Disable();
		}

		if (!Lvl2_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_2").child("enemy"), enemyListLevel2);
			CreateItems(configParameters.child("entities").child("items_lvl_2").child("item"), itemListLevel2);
			for (const auto& enemy : enemyListLevel2) {
				enemy->Start();
			}
			for (const auto& item : itemListLevel2) {
				item->Start();
			}
			Lvl2_Enemies_created = true;
			Lvl2_Items_created = true;
		}

		for (const auto& enemy : enemyListLevel2) {
			enemy->Enable();
		}
		for (const auto& item : itemListLevel2) {
			item->Enable();
		}
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

		for (const auto& enemy : enemyListLevel2) {
			enemy->Disable();
		}
		for (const auto& enemy : enemyListLevel1) {
			enemy->Enable();
		}

		for (const auto& item : itemListLevel2) {
			item->Disable();
		}
		for (const auto& item : itemListLevel1) {
			item->Enable();
		}
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		player->ResetPlayer(current_level);
	}

	if (current_level == 1 && player->position.getX() >= 6520) {
		AdvanceLevel();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

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

	if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_P) == KEY_REPEAT) {
		if (current_level == 1) {
			if (enemyListLevel1[enemyIndex1]->active) {
				enemyListLevel1[enemyIndex1]->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
				enemyListLevel1[enemyIndex1]->ResetPath();
			}
		}
		else if (current_level == 2) {
			if (enemyListLevel2[enemyIndex2]->active) {
				enemyListLevel2[enemyIndex2]->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
				enemyListLevel2[enemyIndex2]->ResetPath();
			}
		}
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		if (current_level == 1) {
			enemyIndex1++;
			if (enemyIndex1 >= numEnemies1) {
				enemyIndex1 = 0;
			}
		} else if (current_level == 2) {
			enemyIndex2++;
			if (enemyIndex2 >= numEnemies2) {
				enemyIndex2 = 0;
			}
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

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Press Gui Control: %d", control->id);

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
	if (current_level == 1) {
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_1").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			Vector2D enemyPos = Vector2D(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());

			bool enemyActive = enemyNode.attribute("active").as_bool();

			for (const auto& enemy : enemyListLevel1) {
				if (enemy && enemy->enemyId == enemyNode.attribute("id").as_string()) {
					if (!enemy->active && enemyActive) {
						enemy->Enable();
					}
					if (enemyActive) {
						enemy->SetPosition(enemyPos);
					}
				}
			}

			for (const auto& enemy : enemyListLevel2) {
				enemy->Disable();
			}

			for (const auto& enemy : enemyListLevel1) {
				enemy->Enable();
			}
		}
	}
	if (current_level == 2) {
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_2").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			Vector2D enemyPos = Vector2D(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());

			bool enemyActive = enemyNode.attribute("active").as_bool();

			for (const auto& enemy : enemyListLevel2) {
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

		for (const auto& enemy : enemyListLevel1) {
			enemy->Disable();
		}

		if (!Lvl2_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_2").child("enemy"), enemyListLevel2);
			for (const auto& enemy : enemyListLevel2) {
				enemy->Start();
			}
			Lvl2_Enemies_created = true;
		}

		for (const auto& enemy : enemyListLevel2) {
			enemy->Enable();
		}
	}

	//items
	if (current_level == 1) {
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("enemies_lvl_1").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			Vector2D itemPos = Vector2D(itemNode.attribute("x").as_float(), itemNode.attribute("y").as_float());

			bool itemActive = itemNode.attribute("active").as_bool();

			for (const auto& item : itemListLevel1) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (!item->active && itemActive) {
						item->Enable();
					}
					if (itemActive) {
						item->SetPosition(itemPos);
					}
				}
			}

			for (const auto& item : itemListLevel2) {
				item->Disable();
			}

			for (const auto& item : itemListLevel1) {
				item->Enable();
			}
		}
	}
	if (current_level == 2) {
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_2").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			Vector2D itemPos = Vector2D(itemNode.attribute("x").as_float(), itemNode.attribute("y").as_float());

			bool itemActive = itemNode.attribute("active").as_bool();

			for (const auto& item : itemListLevel2) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (!item->active && itemActive) {
						item->Enable();
					}
					if (itemActive) {
						item->SetPosition(itemPos);
					}
				}
			}
		}

		for (const auto& item : itemListLevel1) {
			item->Disable();
		}

		if (!Lvl2_Items_created) {
			CreateItems(configParameters.child("entities").child("items_lvl_2").child("item"), itemListLevel2);
			for (const auto& item : itemListLevel2) {
				item->Start();
			}
			Lvl2_Items_created = true;
		}

		for (const auto& item : itemListLevel2) {
			item->Enable();
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
	if (current_level == 1) {
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_1").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			bool enemyActive = enemyNode.attribute("active").as_bool();

			for (const auto& enemy : enemyListLevel1) {
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
	if (current_level == 2) {
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_2").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			bool enemyActive = enemyNode.attribute("active").as_bool();

			for (const auto& enemy : enemyListLevel2) {
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

	//items
	if (current_level == 1) {
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_1").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			bool itemActive = itemNode.attribute("active").as_bool();

			for (const auto& item : itemListLevel1) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (itemActive) {
						if (!item->active) {
							itemNode.attribute("active").set_value("false");
						}
						else {
							itemNode.attribute("x").set_value(item->GetPosition().getX());
							itemNode.attribute("y").set_value(item->GetPosition().getY());
						}
					}
				}
			}
		}
	}
	if (current_level == 2) {
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_2").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			bool itemActive = itemNode.attribute("active").as_bool();

			for (const auto& item : itemListLevel2) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (itemActive) {
						if (!item->active) {
							itemNode.attribute("active").set_value("false");
						}
						else {
							itemNode.attribute("x").set_value(item->GetPosition().getX());
							itemNode.attribute("y").set_value(item->GetPosition().getY());
						}
					}
				}
			}
		}
	}
	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}

void Scene::AdvanceLevel()
{
	if (current_level == 1) {

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

		for (const auto& enemy : enemyListLevel1) {
			enemy->Disable();
		}

		if (!Lvl2_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_2").child("enemy"), enemyListLevel2);

			for (const auto& enemy : enemyListLevel2) {
				enemy->Start();
			}
			Lvl2_Enemies_created = true;
		}

		for (const auto& enemy : enemyListLevel2) {
			enemy->Enable();
		}
	}
}

void Scene::CreateEnemies(pugi::xml_node enemyNode, std::vector<Enemy*>& enemyList)
{
	while (enemyNode) {
		std::string name = enemyNode.attribute("name").as_string();
		Enemy* enemy = nullptr;

		if (name == "boar") {
			enemy = (Boar*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOAR);
		}
		else if (name == "octopus") {
			enemy = (Octopus*)Engine::GetInstance().entityManager->CreateEntity(EntityType::OCTOPUS);
		}
		else if (name == "bee") {
			enemy = (Bee*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BEE);
		}
		else if (name == "hedgehog") {
			enemy = (Hedgehog*)Engine::GetInstance().entityManager->CreateEntity(EntityType::HEDGEHOG);
		}

		if (enemy) {
			enemy->SetParameters(enemyNode);
			enemyList.push_back(enemy);
		}

		enemyNode = enemyNode.next_sibling("enemy");
	}
}

void Scene::CreateItems(pugi::xml_node itemNode, std::vector<Item*>& itemList)
{
	while (itemNode) {
		std::string name = itemNode.attribute("name").as_string();
		Item* item = nullptr;

		if (name == "coin") {
			item = (Item*)Engine::GetInstance().entityManager->CreateItem(ItemType::COIN);
		}
		else if (name == "shuriken") {
			item = (Item*)Engine::GetInstance().entityManager->CreateItem(ItemType::SHURIKEN);
		}
		else if (name == "health") {
			item = (Item*)Engine::GetInstance().entityManager->CreateItem(ItemType::HEALTH);
		}
		else if (name == "ignis") {
			item = (Item*)Engine::GetInstance().entityManager->CreateItem(ItemType::IGNIS);
		}

		if (item) {
			item->SetParameters(itemNode);
			itemList.push_back(item);
		}
		itemNode = itemNode.next_sibling("item");
	}
}
