#pragma once

#include "Item.h"
#include "Entity.h"
#include "SDL2/SDL.h"

struct SDL_Texture;

class ItemShuriken : public Item
{
public:

	ItemShuriken() : Item("Assets/Textures/Items/goldCoin.png",13,13,ItemType::SHURIKEN){}

	~ItemShuriken() {}
};
