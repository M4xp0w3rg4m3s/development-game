#pragma once

#include "Module.h"
#include "Player.h"
#include "Parallax.h"
#include "ParallaxTwoPoints.h"
#include <box2d/box2d.h>
#include "GuiControlButton.h"

class Item;
class Enemy;
class Boss;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void CameraReset();

	Vector2D GetPlayerPosition() const;

	Player* GetPlayer() const;
	Boss* GetBoss() const;

	std::string GetTilePosDebug() {
		return tilePosDebug;

	}

	int GetCurrentLevel()const;

	void LoadState();

	void SaveState();

	bool OnGuiMouseClickEvent(GuiControl* control);

	void AdvanceLevel();

	void CreateEnemies(pugi::xml_node enemyNode, std::vector<Enemy*>& enemyList);
	void CreateItems(pugi::xml_node itemNode, std::vector<Item*>& itemList);

	void CreateButtons();

	bool gameStarted = false;
	bool goingToLvl1 = false;
	bool goingToLvl2 = false;
	bool goingToLvl3 = false;

	bool bossMusicPlayed = false;

	bool pausePressed = false;
	bool debug = false;
private:
	int enemyIndex1 = 0;
	int enemyIndex2 = 0;
	int enemyIndex3 = 0;

	std::string tilePosDebug = "[0,0]";
	bool once = false;

	bool Lvl2_Enemies_created = false;
	bool Lvl3_Enemies_created = false;

	bool Lvl2_Items_created = false;
	bool Lvl3_Items_created = false;

	Player* player = nullptr;
	Parallax* parallax = nullptr;
	ParallaxTwoPoints* parallaxTwoPoints = nullptr;
	Enemy* boss = nullptr;
	
	std::vector<Enemy*> enemyListLevel1;
	std::vector<Enemy*> enemyListLevel2;
	std::vector<Enemy*> enemyListLevel3;

	std::vector<Item*> itemListLevel1;
	std::vector<Item*> itemListLevel2;
	std::vector<Item*> itemListLevel3;

	int current_level = 0;

	int numEnemies1 = 13;
	int numEnemies2 = 11;
	int numEnemies3 = 12;

	GuiControlButton* optionsBt = nullptr;

	int checkpointFx = -1;
	int golemSpawnFx = -1;
	int golemRoarFx = -1;
};