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
}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	parallax = Engine::GetInstance().parallax.get();

	SDL_Rect btPos = { 854 - 65, 0, 65, 20 };
	guiBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, " Options ", btPos, this);

	//Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);

	//Get the player texture name from the config file and assigns the value
	player->textureName = configParameters.child("player").attribute("texturePath").as_string();


	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	player->Start();

	caveBg = Engine::GetInstance().textures.get()->Load("Assets/Maps/background_final1.png");

	LoadState();
	LoadLevel(current_level);

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
    // Camera Update
    if (player->position.getX() > Engine::GetInstance().window.get()->width / 2) {
        Engine::GetInstance().render.get()->camera.x = (int)(-((player->position.getX() + player->width / 2) - (Engine::GetInstance().window.get()->width) / 2));
    }
    else {
        Engine::GetInstance().render.get()->camera.x = 0;
    }

	if (current_level == 1) {
		Engine::GetInstance().render->DrawTexture(caveBg, 0, 0);
	}

    // Level 1
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN && current_level != 1) {
		LoadLevel(1);
    }
    // Level 2
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && current_level != 2) {
		LoadLevel(2);        
    }

    // Level 3
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && current_level != 3) {
		LoadLevel(3);
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN) {
       player->ResetPlayer(current_level);
    }

    // Level advancement logic
    if ((current_level == 1 && player->position.getX() >= 6520) || (current_level == 2 && player->position.getX() >= 4250)) {
        AdvanceLevel();
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
        debug = !debug;

    // Get mouse position and obtain the map coordinate
    Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
    Vector2D mouseTile = Engine::GetInstance().map.get()->WorldToMap((int)mousePos.getX() - Engine::GetInstance().render.get()->camera.x, (int)mousePos.getY() - Engine::GetInstance().render.get()->camera.y);

    // Save the tile position for debugging
    if (mouseTile.getX() >= 0 && mouseTile.getY() >= 0 || once) {
        tilePosDebug = "[" + std::to_string((int)mouseTile.getX()) + "," + std::to_string((int)mouseTile.getY()) + "] ";
        once = true;
    }

    if (Engine::GetInstance().input.get()->GetMouseButtonDown(1) == KEY_DOWN && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_P) == KEY_REPEAT) {
        if (current_level == 1 && enemyListLevel1[enemyIndex1]->active) {
            enemyListLevel1[enemyIndex1]->SetPosition(Vector2D(mouseTile.getX(), mouseTile.getY()));
            enemyListLevel1[enemyIndex1]->ResetPath();
        }
        else if (current_level == 2 && enemyListLevel2[enemyIndex2]->active) {
            enemyListLevel2[enemyIndex2]->SetPosition(Vector2D(mouseTile.getX(), mouseTile.getY()));
            enemyListLevel2[enemyIndex2]->ResetPath();
        }
    }

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN) {
        if (current_level == 1) {
            enemyIndex1++;
            if (enemyIndex1 >= numEnemies1) enemyIndex1 = 0;
        }
        else if (current_level == 2) {
            enemyIndex2++;
            if (enemyIndex2 >= numEnemies2) enemyIndex2 = 0;
        }
    }

    for (auto checkpoint : Engine::GetInstance().map->checkpoints) {
        if ((current_level == 1 && player->position.getX() > checkpoint->pos.getX() && checkpoint->level == 1 && !checkpoint->activated) || 
            (current_level == 2 && player->position.getX() > checkpoint->pos.getX() && checkpoint->level == 2 && !checkpoint->activated)) {
            checkpoint->activated = true;
            SaveState();
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

	if (current_level = 1 && caveBg != nullptr) {
		//Engine::GetInstance().textures.get()->UnLoad(caveBg);
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

int Scene::GetCurrentLevel() const
{
	return current_level;
}

void Scene::LoadState()
{
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (!result) {
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");
	int level = sceneNode.child("level").attribute("currentlevel").as_int();

	if (level != current_level) {
		const char* levelMusic = "Assets/Audio/Music/Background_Level1.wav";
		const char* levelMap = (level == 2) ? "Level2Map.tmx" : "Level1Map.tmx";
		const char* texturePrefix = (level == 2) ? "layers2" : "layers";

		Engine::GetInstance().audio->PlayMusic(levelMusic);
		Engine::GetInstance().map->CleanUp();
		Engine::GetInstance().map->Load("Assets/Maps/", levelMap, true);

		parallax->textureName1 = configParameters.child(texturePrefix).child("one").attribute("texturePath").as_string();
		parallax->textureName2 = configParameters.child(texturePrefix).child("two").attribute("texturePath").as_string();
		parallax->textureName3 = configParameters.child(texturePrefix).child("three").attribute("texturePath").as_string();
		parallax->textureName4 = configParameters.child(texturePrefix).child("four").attribute("texturePath").as_string();
		parallax->textureName5 = configParameters.child(texturePrefix).child("five").attribute("texturePath").as_string();

		parallax->ChangeTextures();
		current_level = level;
		player->ResetPlayer(current_level);
	}

	Vector2D playerPos = Vector2D(sceneNode.child("player").attribute("x").as_float(), sceneNode.child("player").attribute("y").as_float());
	player->SetPosition(playerPos);

	// Lambda to update enemy entities
	auto updateEnemies = [&](int level, auto& enemyList, const std::string& entityPrefix) {
		for (pugi::xml_node entityNode = sceneNode.child("entities").child(entityPrefix.c_str()).child("enemy"); entityNode; entityNode = entityNode.next_sibling("entity")) {
			Vector2D entityPos = Vector2D(entityNode.attribute("x").as_float(), entityNode.attribute("y").as_float());
			bool entityActive = entityNode.attribute("active").as_bool();

			for (Enemy* enemy : enemyList) {
				if (enemy && enemy->enemyId == entityNode.attribute("id").as_string()) {
					if (!enemy->active && entityActive) {
						enemy->Enable();
					}
					if (entityActive) {
						enemy->SetPosition(entityPos);
					}
					else {
						enemy->Disable();
					}
				}
			}
		}
		};

	// Lambda to update item entities
	auto updateItems = [&](int level, auto& itemList, const std::string& entityPrefix) {
		for (pugi::xml_node entityNode = sceneNode.child("entities").child(entityPrefix.c_str()).child("item"); entityNode; entityNode = entityNode.next_sibling("entity")) {
			Vector2D entityPos = Vector2D(entityNode.attribute("x").as_float(), entityNode.attribute("y").as_float());
			bool entityActive = entityNode.attribute("active").as_bool();

			for (Item* item : itemList) {
				if (item && item->itemId == entityNode.attribute("id").as_string()) {
					if (!item->active && entityActive) {
						item->Enable();
					}
					if (entityActive) {
						item->SetPosition(entityPos);
					}
					else {
						item->Disable();
					}
				}
			}
		}
		};


	auto disableOtherEntities = [&](auto& entityListLevel, auto& otherEntityListLevel) {
		for (const auto& entity : otherEntityListLevel) {
			entity->Disable();
		}
		for (const auto& entity : entityListLevel) {
			entity->Enable();
		}
		};

	if (current_level == 1) {
		updateEnemies(1, enemyListLevel1, "enemies_lvl_1");
		updateItems(1, itemListLevel1, "items_lvl_1");
		//disableOtherEntities(itemListLevel2, enemyListLevel2);

	}
	else if (current_level == 2) {
		updateEnemies(2, enemyListLevel2, "enemies_lvl_2");
		updateItems(2, itemListLevel2, "items_lvl_2");
		//disableOtherEntities(itemListLevel1, enemyListLevel1);
	}
}


void Scene::SaveState()
{
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (!result)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	sceneNode.child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("player").attribute("y").set_value(player->GetPosition().getY());

	sceneNode.child("level").attribute("currentlevel").set_value(current_level);

	// Lambda to save enemy entities
	auto saveEnemies = [&](int level, auto& enemyList, const std::string& entityPrefix) {
		for (pugi::xml_node entityNode = sceneNode.child("entities").child(entityPrefix.c_str()).child("enemy"); entityNode; entityNode = entityNode.next_sibling("enemy"))
		{
			for (Enemy* enemy : enemyList) {
				if (enemy && enemy->enemyId == entityNode.attribute("id").as_string()) {
					if (!enemy->active) {
						entityNode.attribute("active").set_value("false");
					}
					else {
						entityNode.attribute("active").set_value("true");
						entityNode.attribute("x").set_value(enemy->GetPosition().getX());
						entityNode.attribute("y").set_value(enemy->GetPosition().getY());
					}
				}
			}
		}
		};

	// Lambda to save item entities
	auto saveItems = [&](int level, auto& itemList, const std::string& entityPrefix) {
		for (pugi::xml_node entityNode = sceneNode.child("entities").child(entityPrefix.c_str()).child("item"); entityNode; entityNode = entityNode.next_sibling("item"))
		{
			for (Item* item : itemList) {
				if (item && item->itemId == entityNode.attribute("id").as_string()) {
					if (!item->active) {
						entityNode.attribute("active").set_value("false");
					}
					else {
						entityNode.attribute("active").set_value("true");
						entityNode.attribute("x").set_value(item->GetPosition().getX());
						entityNode.attribute("y").set_value(item->GetPosition().getY());
					}
				}
			}
		}
		};

	// Save data for current level entities
	if (current_level == 1) {
		saveEnemies(current_level, enemyListLevel1, "enemies_lvl_1");
		saveItems(current_level, itemListLevel1, "items_lvl_1");

	}
	else if (current_level == 2) {
		saveEnemies(current_level, enemyListLevel2, "enemies_lvl_2");
		saveItems(current_level, itemListLevel2, "items_lvl_2");
	}

	loadFile.save_file("config.xml");
}


void Scene::LoadLevel(int level) {
	// Map loading
	const char* mapFile = (level == 1) ? "Level1Map.tmx" : (level == 2) ? "Level2Map.tmx" : "Level3Map.tmx";
	Engine::GetInstance().map->CleanUp();
	Engine::GetInstance().map->Load("Assets/Maps/", mapFile, true);

	// Parallax textures
	const char* texturePrefix = (level == 1) ? "layers" : (level == 2) ? "layers2" : "layers3";
	parallax->textureName1 = configParameters.child(texturePrefix).child("one").attribute("texturePath").as_string();
	parallax->textureName2 = configParameters.child(texturePrefix).child("two").attribute("texturePath").as_string();
	parallax->textureName3 = configParameters.child(texturePrefix).child("three").attribute("texturePath").as_string();
	parallax->textureName4 = configParameters.child(texturePrefix).child("four").attribute("texturePath").as_string();
	parallax->textureName5 = configParameters.child(texturePrefix).child("five").attribute("texturePath").as_string();
	parallax->ChangeTextures();

	// Disable enemies/items from other levels
	DisableEnemiesAndItems(level);

	// Create and enable level-specific enemies/items
	if (level == 1) {
		if (!Lvl1_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_1").child("enemy"), enemyListLevel1);
			for (const auto& enemy : enemyListLevel1) enemy->Start();
			Lvl1_Enemies_created = true;
		}
		for (const auto& enemy : enemyListLevel1) enemy->Enable();

		if (!Lvl1_Items_created) {
			CreateItems(configParameters.child("entities").child("items_lvl_1").child("item"), itemListLevel1);
			for (const auto& item : itemListLevel1) item->Start();
			Lvl1_Items_created = true;
		}
		for (const auto& item : itemListLevel1) item->Enable();
	}
	else if (level == 2) {
		if (!Lvl2_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_2").child("enemy"), enemyListLevel2);
			for (const auto& enemy : enemyListLevel2) enemy->Start();
			Lvl2_Enemies_created = true;
		}
		for (const auto& enemy : enemyListLevel2) enemy->Enable();

		if (!Lvl2_Items_created) {
			CreateItems(configParameters.child("entities").child("items_lvl_2").child("item"), itemListLevel2);
			for (const auto& item : itemListLevel2) item->Start();
			Lvl2_Items_created = true;
		}
		for (const auto& item : itemListLevel2) item->Enable();
	}
	else if (level == 3) {
		if (!Lvl3_Enemies_created) {
			CreateEnemies(configParameters.child("entities").child("enemies_lvl_3").child("enemy"), enemyListLevel3);
			for (const auto& enemy : enemyListLevel3) enemy->Start();
			Lvl3_Enemies_created = true;
		}
		for (const auto& enemy : enemyListLevel3) enemy->Enable();

		if (!Lvl3_Items_created) {
			CreateItems(configParameters.child("entities").child("items_lvl_3").child("item"), itemListLevel3);
			for (const auto& item : itemListLevel3) item->Start();
			Lvl3_Items_created = true;
		}
		for (const auto& item : itemListLevel3) item->Enable();
	}

	current_level = level;
	player->ResetPlayer(current_level);
}


void Scene::DisableEnemiesAndItems(int level) {
	if (level != 1) {
		if (Lvl1_Enemies_created) { for (const auto& enemy : enemyListLevel1) enemy->Disable(); }
		if (Lvl1_Items_created) { for (const auto& item : itemListLevel1) item->Disable(); }
	}
	if (level != 2) {
		if (Lvl2_Enemies_created) for (const auto& enemy : enemyListLevel2) enemy->Disable();
		if (Lvl2_Items_created) for (const auto& item : itemListLevel2) item->Disable();
	}
	if (level != 3) {
		if (Lvl3_Enemies_created) for (const auto& enemy : enemyListLevel3) enemy->Disable();
		if (Lvl3_Items_created) for (const auto& item : itemListLevel3) item->Disable();
	}
}

void Scene::AdvanceLevel() {
	if (current_level == 1) {
		LoadLevel(2);
	}
	else if (current_level == 2) {
		LoadLevel(3);
	}
}

void Scene::CreateEnemies(pugi::xml_node enemyNode, std::vector<Enemy*>& enemyList)
{
	while (enemyNode) {
		std::string name = enemyNode.attribute("name").as_string();
		Enemy* enemy = nullptr;

		// Create the enemy based on its name
		if (name == "boar") {
			enemy = (Boar*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOAR);
			if (!enemy) {
				LOG("Failed to create enemy of type BOAR!");
			}
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
		}

		// If the enemy is successfully created, set its parameters
		if (enemy) {
			enemy->SetParameters(enemyNode);

			// Check if the enemy is active, and set it accordingly
			bool isActive = enemyNode.attribute("active").as_bool(true);  // Default to true if attribute is missing or invalid
			if (!isActive) {
				enemy->Disable();  // Disable the enemy if inactive
			}

			enemyList.push_back(enemy);
		}

		enemyNode = enemyNode.next_sibling("enemy");  // Move to the next enemy node
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