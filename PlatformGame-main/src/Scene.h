#pragma once

#include "Module.h"
#include "Player.h"
#include <box2d/box2d.h>

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

	// Load Parameters from config file
	bool LoadParameters(xml_node parameters);

	b2World* world;

private:
	Player* player;
	xml_node configParameters;
};