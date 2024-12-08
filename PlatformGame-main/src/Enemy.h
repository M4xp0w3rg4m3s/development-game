#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Sprite.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy(EntityType type);
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	Vector2D GetCenterPosition();

	void ResetPath();

	void SetPathfindingType(EnemyType type);

	virtual void GoToPath();

	void DeleteThis();

	std::string enemyId;

protected:
	bool debug = false;

	std::string textureName;
	SDL_Texture* texture = nullptr;
	int texW = 0, texH = 0;
	int width = 0, height = 0;

	pugi::xml_node parameters;

	Sprite* animator = nullptr;
	PhysBody* pbody = nullptr;
	Pathfinding* pathfinding = nullptr;
};
