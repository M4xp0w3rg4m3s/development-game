#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include <box2d/box2d.h>
#include "Entity.h"
#include "Sprite.h"

class Projectile : public Entity{
public:

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

	void SetAnimation(int id);

	void SetGravity(float gravity);
private:
	Sprite* animator;
	std::string textureName;
	SDL_Texture* texture = NULL;

	int texW = 0, texH = 0;
	int width = 13;
	int height = 13;

	b2Vec2 position;
	b2Vec2 direction;

	PhysBody* body;

	int speed = 15;

	Timer projectileDeadTimer;
	int projectileDeadTime = 1;
};