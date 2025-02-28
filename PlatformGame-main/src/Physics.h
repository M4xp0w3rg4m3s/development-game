#pragma once
#include "Module.h"
#include "Entity.h"
#include "box2d/box2d.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -10.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

#include <list>

// types of bodies
enum bodyType {
	DYNAMIC,
	STATIC,
	KINEMATIC
};

enum class ColliderType {
	PLAYER,
	ENEMY,
	GROUND_CHECK,
	PLAYER_ATTACK_LEFT,
	PLAYER_ATTACK_RIGHT,
	BOSS,
	BOSS_ATTACK_LEFT,
	BOSS_ATTACK_RIGHT,
	ITEM,
	BOULDER,
	PLATFORM,
	KILL,
	PROJECTILE_PLAYER,
	PROJECTILE_ENEMY,
	UNKNOWN,
	// ..
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL), ctype(ColliderType::UNKNOWN)
	{}

	~PhysBody() {}

	void GetPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

	void CreateWeld(PhysBody* pbody, Vector2D position);

public:
	int activeCollisions = 0;

	int width = 0;
	int height = 0;
	b2Body* body = nullptr;
	Entity* listener = nullptr;
	ColliderType ctype;
};

// Module --------------------------------------
class Physics : public Module, public b2ContactListener // TODO
{
public:

	// Constructors & Destructors
	Physics();
	~Physics();

	// Main module steps
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	// Create basic physics objects
	PhysBody* CreateRectangle(int x, int y, int width, int height, bodyType type);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, bodyType type);
	PhysBody* CreateCircle(int x, int y, int radious, bodyType type);
	PhysBody* CreateCircleSensor(int x, int y, int radious, bodyType type);
	PhysBody* CreateChain(int x, int y, int* points, int size, bodyType type);

	PhysBody* CreatePlayer(int x, int y, int width, int height, bodyType type);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);

	void DeletePhysBody(PhysBody* physBody);
	bool IsPendingToDelete(PhysBody* physBody);

	b2World* GetWorld() const {
		return world;
	}
	uint16_t playerAttackLayer = 0x0001;
	uint16_t EnemyLayer = 0x0002;
private:

	// Debug mode
	bool debug = false;

	// Box2D World
	b2World* world = nullptr;

	// List of bodies for deleting them later
	std::list<PhysBody*> bodiesToDelete;
};