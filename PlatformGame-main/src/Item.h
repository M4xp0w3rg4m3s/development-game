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
	Item(ItemType currentType);
	virtual ~Item();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}
	void SetPosition(Vector2D pos);

	ItemType GetCurrentType();
	Vector2D GetPosition();

	std::string itemId;

public:

	bool isPicked = false;

private:
	ItemType currentType;

	Sprite* animator = nullptr;
	SDL_Texture* texture = nullptr;
	std::string textureName;
	int texW = 0, texH = 0;

	pugi::xml_node parameters;

	//Add a physics to an item
	PhysBody* pbody = nullptr;
};
