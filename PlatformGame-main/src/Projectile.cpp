#include "Projectile.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"

Projectile::Projectile(b2Vec2 position, b2Vec2 direction) : Entity(EntityType::PROJECTILE)
{
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Shuriken_test.png");
	animator = new Sprite(texture);

	body = Engine::GetInstance().physics.get()->CreateCircle((int)position.x, (int)position.y, width/2, bodyType::DYNAMIC);

	texW = width, texH = height;
	
	this->position = position;
	this->direction = direction;
	
}

Projectile::~Projectile()
{
	delete animator;
}

bool Projectile::Awake()
{
	//Initialize Projectile parameters

	return true;
}

bool Projectile::Start()
{
	animator->SetNumberAnimations(1);
	
	animator->AddKeyFrame(0, { 0 * 8,0 * 8,8,8 });
	/*animator->AddKeyFrame(0, { 0 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 1 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 2 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 3 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 4 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 5 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 6 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 7 * 64,1 * 64,64,64 });*/
	animator->SetAnimationDelay(0, 100);

	animator->SetAnimation(0);
	animator->SetLoop(true);

	// Add physics to the projectile - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	body->body->SetFixedRotation(true);

	b2MassData projectileMass;
	projectileMass.mass = 1.15f;
	projectileMass.center = body->body->GetLocalCenter();
	body->body->SetMassData(&projectileMass);

	body->body->GetFixtureList()[0].SetFriction(0);
	body->body->SetGravityScale(0);

	// Assign projectile class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	body->listener = this;

	// Assign collider type
	body->ctype = ColliderType::PROJECTILE;

	return true;
}

bool Projectile::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, body->body->GetLinearVelocity().y);

	velocity.x = direction.x * speed;
	velocity.y = direction.y * speed;
	
	body->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = body->body->GetTransform();
	position.x = (METERS_TO_PIXELS(pbodyPos.p.x) - width / 2);
	position.y = (METERS_TO_PIXELS(pbodyPos.p.y) - height / 2);

	animator->Update();
	animator->Draw((int)position.x, (int)position.y, 0, 0);
	
	return true;
}

bool Projectile::CleanUp()
{
	LOG("Cleanup Projectile");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(body);
	return true;
}

void Projectile::OnCollision(PhysBody* physA, PhysBody* physB)
{
	
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		
		break;
	case ColliderType::BOULDER:
		CleanUp();
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM PROJECTILE");
		CleanUp();
		break;
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