#pragma once

#include "Module.h"
#include "Player.h"
#include "Parallax.h"
#include <box2d/box2d.h>
#include "Enemy.h"
#include "GuiControlButton.h"

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

	std::string GetTilePosDebug() {
		return tilePosDebug;

	}

	int GetCurrentLevel()const;

	void LoadState();

	void SaveState();

	bool OnGuiMouseClickEvent(GuiControl* control);

	void AdvanceLevel();

	void CreateEnemies(pugi::xml_node enemyNode, std::vector<Enemy*>& enemyList);

	bool debug = false;
private:
	int enemyIndex1 = 0;
	int enemyIndex2 = 0;

	std::string tilePosDebug = "[0,0]";
	bool once = false;

	bool Lvl2_Enemies_created = false;

	SDL_Texture* img;
	SDL_Texture* caveBg;

	Player* player;
	Parallax* parallax;

	std::vector<Enemy*> enemyListLevel1;
	std::vector<Enemy*> enemyListLevel2;

	int current_level = 1;

	int numEnemies1 = 13;
	int numEnemies2 = 11;

	GuiControlButton* guiBt;
};