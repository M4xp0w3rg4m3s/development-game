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

	Projectile();

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
	int width = 16;
	int height = 45;

	Vector2D position;

	PhysBody* body;
};