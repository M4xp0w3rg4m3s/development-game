#include "Projectile.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"

Projectile::Projectile() : Entity(EntityType::PROJECTILE)
{
	animator = new Sprite(texture);

	body = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), width, height, bodyType::DYNAMIC);

	texW = width, texH = height;
}

Projectile::~Projectile()
{
	delete animator;
}

bool Projectile::Awake()
{
	//Initialize Projectile parameters
	position = Vector2D(192, 384);
	return true;
}

bool Projectile::Start()
{
	texture = Engine::GetInstance().textures->Load(textureName.c_str());

	animator->SetNumberAnimations(1);
	
	animator->AddKeyFrame(0, { 0 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 1 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 2 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 3 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 4 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 5 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 6 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 7 * 64,1 * 64,64,64 });
	animator->SetAnimationDelay(0, 100);

	animator->SetAnimation(0);

	// Add physics to the projectile - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	body->body->SetFixedRotation(true);

	b2MassData projectileMass;
	projectileMass.mass = 1.15f;
	projectileMass.center = body->body->GetLocalCenter();
	body->body->SetMassData(&projectileMass);

	body->body->GetFixtureList()[0].SetFriction(0);

	// Assign projectile class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	body->listener = this;

	// Assign collider type
	body->ctype = ColliderType::PROJECTILE;

	return true;
}

bool Projectile::Update(float dt)
{
	return true;
}

bool Projectile::CleanUp()
{
	LOG("Cleanup Projectile");
	return true;
}

void Projectile::OnCollision(PhysBody* physA, PhysBody* physB)
{
	
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		CleanUp();

	case ColliderType::BOULDER:
		CleanUp();

	case ColliderType::PLATFORM:
		CleanUp();

	case ColliderType::KILL:
		LOG("Collision KILL");
		CleanUp();
		break;

	case ColliderType::ITEM:
		LOG("Collision ITEM");
		CleanUp();
		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;

	default:
		break;
	}
}
