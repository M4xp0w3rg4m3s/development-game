#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"

class Sprite;

struct SDL_Texture;

enum class ItemType {
	SHURIKEN,
	IGNIS,
	HEALTH,
	COIN
};

class Item : public Entity
{
public:

	Item();
	Item(const char* texturePath, int texW, int texH, ItemType currentType);
	virtual ~Item();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked = false;

private:
	ItemType currentType;

	Sprite* animator = nullptr;
	SDL_Texture* texture = nullptr;
	const char* texturePath = "";
	int texW, texH;

	//Add a physics to an item
	PhysBody* pbody = nullptr;
};
