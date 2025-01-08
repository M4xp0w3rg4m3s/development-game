#pragma once

#include "Module.h"
#include "Entity.h"
#include <list>
#include "box2d/box2d.h"
#include "item.h"

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake();

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(EntityType type, bool doCalls = false);
	Entity* CreateProjectile( b2Vec2 position, b2Vec2 direction, bool doCalls = false);
	Entity* CreateItem(ItemType type, bool doCalls = false);

	void DestroyEntity(Entity* entity);

	void DeleteEntity(Entity* entity);

	void AddEntity(Entity* entity);

public:

	std::list<Entity*> entities;

	std::list<Entity*> entitiesToDelete;
};
