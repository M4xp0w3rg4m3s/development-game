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

	void ResetPath();

	void SetPathfindingType(EnemyType type);

public:

private:

	std::string textureName;
	SDL_Texture* texture = NULL;
	int texW, texH;
	int width = 48;
	int height = 32;

	pugi::xml_node parameters;

	Sprite* animator = NULL;
	PhysBody* pbody = NULL;
	Pathfinding* pathfinding = NULL;
};
