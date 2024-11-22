#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include <box2d/box2d.h>
#include "Entity.h"
#include "Sprite.h"

class Projectile : Entity{
public:

	enum ProjectileState
	{
		FLYING,
		COLLIDED
	};
	enum ProjectileType
	{
		ENEMY,
		PLAYER
	};

	Projectile(b2Vec2 position, b2Vec2 direction);

	~Projectile();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

private:
	Sprite* animator;
	std::string textureName;
	SDL_Texture* texture = NULL;

	int texW = 0, texH = 0;
	int width = 8;
	int height = 8;

	b2Vec2 position;
	b2Vec2 direction;

	PhysBody* body;

	int speed = 10;
};