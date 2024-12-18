#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Item.h"
#include "Boulder.h"
#include "Enemy.h"
#include "Projectile.h"

#include "Boar.h"
#include "Hedgehog.h"
#include "Octopus.h"
#include "Bee.h"

EntityManager::EntityManager() : Module()
{
	name = "entitymanager";
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake()
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;

	for(const auto entity : entities)
	{

		ret = entity->CleanUp();
	}

	entities.clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type, bool doCalls)
{
	Entity* entity = nullptr; 
	//L03: TODO 3a: Instantiate entity according to the type and add the new entity to the list of Entities
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::BOULDER:
		entity = new Boulder();
		break;
	case EntityType::BOAR:
		entity = new Boar();
		break;
	case EntityType::BEE:
		entity = new Bee();
		break;
	case EntityType::HEDGEHOG:
		entity = new Hedgehog();
		break;
	case EntityType::OCTOPUS:
		entity = new Octopus();
		break;
	default:
		break;
	}

	if (doCalls) {
		entity->Awake();
		entity->Start();
	}

	entities.push_back(entity);

	return entity;
}

Entity* EntityManager::CreateProjectile(b2Vec2 position, b2Vec2 direction, bool doCalls)
{
	Entity* entity = nullptr;

	entity = new Projectile(position,direction);

	if (doCalls) {
		entity->Awake();
		entity->Start();
	}

	entities.push_back(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		if (*it == entity) {
			(*it)->CleanUp();
			delete* it; // Free the allocated memory
			entities.erase(it); // Remove the entity from the list
			break; // Exit the loop after removing the entity
		}
	}
}

void EntityManager::DeleteEntity(Entity* entity)
{
	entitiesToDelete.push_back(entity);
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.push_back(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Update(dt);
	}
	return ret;
}

bool EntityManager::PostUpdate()
{
	for (Entity* entity : entitiesToDelete) {
		Engine::GetInstance().entityManager->DestroyEntity(entity);
	}
	entitiesToDelete.clear();

	return true;
}
