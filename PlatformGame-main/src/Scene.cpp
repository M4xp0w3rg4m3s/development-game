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
#include "Enemy.h"
#include <string>

#include "Boar.h"
#include "Octopus.h"
#include "Bee.h"
#include "Hedgehog.h"
#include "Boss.h"

#include "GuiControl.h"
#include "GuiManager.h"

Scene::Scene() : Module()
{
	name = "scene";
}

// Destructor
Scene::~Scene()
{
	// enemies
	enemyListLevel1.clear();
	enemyListLevel2.clear();
	enemyListLevel3.clear();

	// items
	itemListLevel1.clear();
	itemListLevel2.clear();
	itemListLevel3.clear();
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

	parallaxTwoPoints = Engine::GetInstance().parallaxTwoPoints.get();

	parallaxTwoPoints->textureName1 = configParameters.child("layersCave").child("one").attribute("texturePath").as_string();
	parallaxTwoPoints->textureName2 = configParameters.child("layersCave").child("two").attribute("texturePath").as_string();
	parallaxTwoPoints->textureName3 = configParameters.child("layersCave").child("three").attribute("texturePath").as_string();
	parallaxTwoPoints->textureName4 = configParameters.child("layersCave").child("four").attribute("texturePath").as_string();
	parallaxTwoPoints->textureName5 = configParameters.child("layersCave").child("five").attribute("texturePath").as_string();

	//Get the map name from the config file and assigns the value
	Engine::GetInstance().map.get()->mapName = configParameters.child("map").attribute("name").as_string();
	Engine::GetInstance().map.get()->mapPath = configParameters.child("map").attribute("path").as_string();

	//Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);

	//Get the player texture name from the config file and assigns the value
	player->textureName = configParameters.child("player").attribute("texturePath").as_string();

	current_level = 1;

	CreateEnemies(configParameters.child("entities").child("enemies_lvl_1").child("enemy"), enemyListLevel1);
	CreateItems(configParameters.child("entities").child("items_lvl_1").child("item"), itemListLevel1);

	CreateButtons();

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	Engine::GetInstance().map->Load("Assets/Maps/", "Level1Map.tmx");

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
	if (!gameStarted) {
		goingToLvl1 = true;
		gameStarted = true;
	}

	if (optionsBt->state == GuiControlState::PRESSED)
	{
		settingsPressed = true;
	}
	if (settingsPressed)
	{
		settingsPressed = false;
		Engine::GetInstance().guiManager->DeleteButtons();
		Engine::GetInstance().ChangeLoopState(LoopState::SETTINGS);
	}

	if (player->position.getX() > Engine::GetInstance().window.get()->width / 2) {
		Engine::GetInstance().render.get()->camera.x = (int)(-((player->position.getX() + player->width / 2) - (Engine::GetInstance().window.get()->width) / 2));
	}
	else {
		Engine::GetInstance().render.get()->camera.x = 0;
	}

	if (current_level == 1) {
		parallaxTwoPoints->SetRange(0, 1024);
		parallaxTwoPoints->textureName1 = configParameters.child("layersCave").child("one").attribute("texturePath").as_string();
		parallaxTwoPoints->textureName2 = configParameters.child("layersCave").child("two").attribute("texturePath").as_string();
		parallaxTwoPoints->textureName3 = configParameters.child("layersCave").child("three").attribute("texturePath").as_string();
		parallaxTwoPoints->textureName4 = configParameters.child("layersCave").child("four").attribute("texturePath").as_string();
		parallaxTwoPoints->textureName5 = configParameters.child("layersCave").child("five").attribute("texturePath").as_string();
		parallaxTwoPoints->ChangeTextures();
	}
	if (current_level == 2) {
		parallaxTwoPoints->SetRange(-1, -1);
	}
	if (current_level == 3) {
		parallaxTwoPoints->SetRange(3590, 7700);
		parallaxTwoPoints->textureName1 = configParameters.child("layersCave").child("one").attribute("texturePath").as_string();
		parallaxTwoPoints->textureName2 = configParameters.child("layersCave").child("two").attribute("texturePath").as_string();
		parallaxTwoPoints->textureName3 = configParameters.child("layersCave").child("three").attribute("texturePath").as_string();
		parallaxTwoPoints->textureName4 = configParameters.child("layersCave").child("four").attribute("texturePath").as_string();
		parallaxTwoPoints->textureName5 = configParameters.child("layersCave").child("four").attribute("texturePath").as_string();
		parallaxTwoPoints->ChangeTextures();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {

		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level3Map.tmx", true);

		parallax->textureName1 = configParameters.child("layers3").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers3").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers3").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers3").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers3").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();

		current_level = 3;

		player->ResetPlayer(current_level);

		for (const auto& enemy : enemyListLevel1) {
			enemy->Disable();
		}
		for (const auto& enemy : enemyListLevel2) {
			enemy->Disable();
		}
		if (!Lvl3_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_3").child("enemy"), enemyListLevel3);
			for (const auto& enemy : enemyListLevel3) {
				enemy->Start();
			}
			Lvl3_Enemies_created = true;
		}
		for (const auto& enemy : enemyListLevel3) {
			enemy->Enable();
		}

		for (const auto& item : itemListLevel1) {
			item->Disable();
		}
		for (const auto& item : itemListLevel2) {
			item->Disable();
		}
		if (!Lvl3_Items_created) {
			CreateItems(configParameters.child("entities").child("items_lvl_3").child("item"), itemListLevel3);
			for (const auto& item : itemListLevel3) {
				item->Start();
			}
			Lvl3_Items_created = true;
		}
		for (const auto& item : itemListLevel3) {
			item->Enable();
		}
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

		current_level = 2;

		player->ResetPlayer(current_level);

		for (const auto& enemy : enemyListLevel1) {
			enemy->Disable();
		}
		for (const auto& enemy : enemyListLevel3) {
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

		for (const auto& item : itemListLevel1) {
			item->Disable();
		}
		for (const auto& item : itemListLevel3) {
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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {

		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level1Map.tmx", true);

		parallax->textureName1 = configParameters.child("layers").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();

		current_level = 1;

		player->ResetPlayer(current_level);

		for (const auto& enemy : enemyListLevel2) {
			enemy->Disable();
		}
		for (const auto& enemy : enemyListLevel3) {
			enemy->Disable();
		}
		for (const auto& enemy : enemyListLevel1) {
			enemy->Enable();
		}

		for (const auto& item : itemListLevel2) {
			item->Disable();
		}
		for (const auto& item : itemListLevel3) {
			item->Disable();
		}
		for (const auto& item : itemListLevel1) {
			item->Enable();
		}
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
		player->ResetPlayer(current_level);
	}

	if (current_level == 1 && player->position.getX() >= 6520 || current_level == 2 && player->position.getX() >= 4250) {
		AdvanceLevel();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	//Get mouse position and obtain the map coordinate
	Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
	Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap((int)mousePos.getX() - Engine::GetInstance().render.get()->camera.x, (int)mousePos.getY() - Engine::GetInstance().render.get()->camera.y);

	Vector2D highlightTile = Engine::GetInstance().map.get()->MapToWorld((int)mouseTile.getX(), (int)mouseTile.getY());
	SDL_Rect rect = { 0,0,32,32 };

	// saves the tile pos for debugging purposes
	if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
		tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
		once = true;
	}

	if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
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
		else if (current_level == 3) {
			if (enemyListLevel3[enemyIndex3]->active) {
				enemyListLevel3[enemyIndex3]->SetPosition(Vector2D(highlightTile.getX(), highlightTile.getY()));
				enemyListLevel3[enemyIndex3]->ResetPath();
			}
		}
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
		if (current_level == 1) {
			enemyIndex1++;
			if (enemyIndex1 >= numEnemies1) {
				enemyIndex1 = 0;
			}
		}
		else if (current_level == 2) {
			enemyIndex2++;
			if (enemyIndex2 >= numEnemies2) {
				enemyIndex2 = 0;
			}
		}
		else if (current_level == 3) {
			enemyIndex3++;
			if (enemyIndex3 >= numEnemies3) {
				enemyIndex3 = 0;
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
		else if (current_level == 3) {
			if (player->position.getX() > checkpoint->pos.getX() && checkpoint->level == 3 && !checkpoint->activated) {
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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
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

	if (optionsBt != nullptr)
	{
		Engine::GetInstance().guiManager->DeleteButton(1);
	}

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

Enemy* Scene::GetBoss() const
{
	return boss;
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

	if (sceneNode.child("level").attribute("currentlevel").as_int() == 2 && current_level != 2) {
		Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");
		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level2Map.tmx", true);
		parallax->textureName1 = configParameters.child("layers2").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers2").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers2").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers2").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers2").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();
		current_level = 2;
		player->ResetPlayer(current_level);

	}
	else if (sceneNode.child("level").attribute("currentlevel").as_int() == 1 && current_level != 1) {
		Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");
		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level1Map.tmx", true);
		parallax->textureName1 = configParameters.child("layers").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();
		current_level = 1;
		player->ResetPlayer(current_level);
	}
	else if (sceneNode.child("level").attribute("currentlevel").as_int() == 3 && current_level != 3) {
		Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/Background_Level1.wav");
		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level3Map.tmx", true);
		parallax->textureName1 = configParameters.child("layers3").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers3").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers3").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers3").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers3").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();
		current_level = 3;
		player->ResetPlayer(current_level);
	}

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("player").attribute("x").as_float(), sceneNode.child("player").attribute("y").as_float());
	player->SetPosition(playerPos);
	int playerIgnis = sceneNode.child("player").attribute("ignis").as_int();
	player->SetPlayerIgnis(playerIgnis);
	int playerLives = sceneNode.child("player").attribute("life").as_int();
	player->SetPlayerLives(playerLives);

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
					if (enemy->active) {
						if (!enemyActive) {
							enemy->Disable();
						}
						else {
							enemy->SetPosition(enemyPos);
						}
					}
				}
			}
			for (const auto& enemy : enemyListLevel2) {
				enemy->Disable();
			}
			for (const auto& enemy : enemyListLevel3) {
				enemy->Disable();
			}
		}
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_1").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			bool itemActive = itemNode.attribute("active").as_bool();
			for (const auto& item : itemListLevel1) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (!item->active && itemActive) {
						item->Enable();
					}
					if (item->active) {
						if (!itemActive) {
							item->Disable();
						}
					}
				}
			}
			for (const auto& item : itemListLevel2) {
				item->Disable();
			}
			for (const auto& item : itemListLevel3) {
				item->Disable();
			}
		}
	}
	if (current_level == 2) {
		if (!Lvl2_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_2").child("enemy"), enemyListLevel2);
			for (const auto& enemy : enemyListLevel2) {
				enemy->Start();
			}
			Lvl2_Enemies_created = true;
		}
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_2").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			Vector2D enemyPos = Vector2D(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());
			bool enemyActive = enemyNode.attribute("active").as_bool();
			for (const auto& enemy : enemyListLevel2) {
				if (enemy && enemy->enemyId == enemyNode.attribute("id").as_string()) {
					if (!enemy->active && enemyActive) {
						enemy->Enable();
					}
					if (enemy->active) {
						if (!enemyActive) {
							enemy->Disable();
						}
						else {
							enemy->SetPosition(enemyPos);
						}
					}
				}
			}
		}
		for (const auto& enemy : enemyListLevel1) {
			enemy->Disable();
		}
		for (const auto& enemy : enemyListLevel3) {
			enemy->Disable();
		}
		if (!Lvl2_Items_created) {
			CreateItems(configParameters.child("entities").child("items_lvl_2").child("item"), itemListLevel2);
			for (const auto& item : itemListLevel2) {
				item->Start();
			}
			Lvl2_Items_created = true;
		}
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_2").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			bool itemActive = itemNode.attribute("active").as_bool();
			for (const auto& item : itemListLevel2) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (!item->active && itemActive) {
						item->Enable();
					}
					if (item->active) {
						if (!itemActive) {
							item->Disable();
						}
					}
				}
			}
			for (const auto& item : itemListLevel1) {
				item->Disable();
			}
			for (const auto& item : itemListLevel3) {
				item->Disable();
			}
		}
	}
	if (current_level == 3) {
		if (!Lvl3_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_3").child("enemy"), enemyListLevel3);
			for (const auto& enemy : enemyListLevel3) {
				enemy->Start();
			}
			Lvl3_Enemies_created = true;
		}
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_3").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			Vector2D enemyPos = Vector2D(enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float());
			bool enemyActive = enemyNode.attribute("active").as_bool();
			for (const auto& enemy : enemyListLevel3) {
				if (enemy && enemy->enemyId == enemyNode.attribute("id").as_string()) {
					if (!enemy->active && enemyActive) {
						enemy->Enable();
					}
					if (enemy->active) {
						if (!enemyActive) {
							enemy->Disable();
						}
						else {
							enemy->SetPosition(enemyPos);
						}
					}
				}
			}
		}
		for (const auto& enemy : enemyListLevel1) {
			enemy->Disable();
		}
		for (const auto& enemy : enemyListLevel2) {
			enemy->Disable();
		}
		if (!Lvl3_Items_created) {
			CreateItems(configParameters.child("entities").child("items_lvl_3").child("item"), itemListLevel3);
			for (const auto& item : itemListLevel3) {
				item->Start();
			}
			Lvl3_Items_created = true;
		}
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_3").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			bool itemActive = itemNode.attribute("active").as_bool();
			for (const auto& item : itemListLevel3) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (!item->active && itemActive) {
						item->Enable();
					}
					if (item->active) {
						if (!itemActive) {
							item->Disable();
						}
					}
				}
			}
			for (const auto& item : itemListLevel1) {
				item->Disable();
			}
			for (const auto& item : itemListLevel2) {
				item->Disable();
			}
			if (!Lvl3_Items_created) {
				CreateItems(configParameters.child("entities").child("items_lvl_3").child("item"), itemListLevel3);
				for (const auto& item : itemListLevel3) {
					item->Start();
				}
				Lvl3_Items_created = true;
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
	sceneNode.child("player").attribute("ignis").set_value(player->GetPlayerIgnis());
	sceneNode.child("player").attribute("life").set_value(player->GetPlayerLives());

	sceneNode.child("level").attribute("currentlevel").set_value(current_level);

	//enemies
	if (current_level == 1) {
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_1").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			for (const auto& enemy : enemyListLevel1) {
				if (enemy && enemy->enemyId == enemyNode.attribute("id").as_string()) {
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
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_1").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			for (const auto& item : itemListLevel1) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (!item->active) {
						itemNode.attribute("active").set_value("false");
					}
				}
			}
		}
	}
	if (current_level == 2) {
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_2").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			for (const auto& enemy : enemyListLevel2) {
				if (enemy && enemy->enemyId == enemyNode.attribute("id").as_string()) {
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
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_2").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			for (const auto& item : itemListLevel2) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (!item->active) {
						itemNode.attribute("active").set_value("false");
					}
				}
			}
		}
	}
	if (current_level == 3) {
		for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies_lvl_3").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
		{
			for (const auto& enemy : enemyListLevel3) {
				if (enemy && enemy->enemyId == enemyNode.attribute("id").as_string()) {
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
		for (pugi::xml_node itemNode = sceneNode.child("entities").child("items_lvl_3").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
		{
			for (const auto& item : itemListLevel3) {
				if (item && item->itemId == itemNode.attribute("id").as_string()) {
					if (!item->active) {
						itemNode.attribute("active").set_value("false");
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

		goingToLvl2 = true;

		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level2Map.tmx", true);

		parallax->textureName1 = configParameters.child("layers2").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers2").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers2").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers2").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers2").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();

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

		for (const auto& item : itemListLevel1) {
			item->Disable();
		}
		for (const auto& item : itemListLevel3) {
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
	if (current_level == 2 && !goingToLvl2) {

		goingToLvl3 = true;

		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", "Level3Map.tmx", true);

		parallax->textureName1 = configParameters.child("layers3").child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child("layers3").child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child("layers3").child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child("layers3").child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child("layers3").child("five").attribute("texturePath").as_string();
		parallax->ChangeTextures();

		current_level = 3;

		player->ResetPlayer(current_level);

		for (const auto& enemy : enemyListLevel2) {
			enemy->Disable();
		}
		if (!Lvl3_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_3").child("enemy"), enemyListLevel3);

			for (const auto& enemy : enemyListLevel3) {
				enemy->Start();
			}
			Lvl3_Enemies_created = true;
		}
		for (const auto& enemy : enemyListLevel3) {
			enemy->Enable();
		}

		for (const auto& item : itemListLevel1) {
			item->Disable();
		}
		for (const auto& item : itemListLevel2) {
			item->Disable();
		}
		if (!Lvl3_Items_created) {
			CreateItems(configParameters.child("entities").child("items_lvl_3").child("item"), itemListLevel3);
			for (const auto& item : itemListLevel3) {
				item->Start();
			}
			Lvl3_Items_created = true;
		}
		for (const auto& item : itemListLevel3) {
			item->Enable();
		}
	}
	SaveState();
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
		else if (name == "boss") {
			enemy = (Boss*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOSS);
			boss = enemy;
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

void Scene::CreateButtons()
{
	SDL_Rect btPos = { 854 - 65, 0, 65, 20 };
	optionsBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, " Options ", btPos, this);
}
