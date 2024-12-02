#pragma once

#include "Module.h"
#include "Player.h"
#include "Parallax.h"
#include <box2d/box2d.h>
#include "Enemy.h"

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

	Vector2D GetPlayerPosition();

	Player* GetPlayer() const;

	std::string GetTilePosDebug() {
		return tilePosDebug;

	}

private:
	int enemyIndex = 0;

	std::string tilePosDebug = "[0,0]";
	bool once = false;

	SDL_Texture* img;
	SDL_Texture* caveBg;

	Player* player;
	Parallax* parallax;

	std::vector<Enemy*> enemyList;

	int current_level = 1;
};