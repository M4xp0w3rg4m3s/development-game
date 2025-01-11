#include "Projectile.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"

Projectile::Projectile(b2Vec2 position, b2Vec2 direction) : Entity(EntityType::PROJECTILE)
{
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Projectiles.png");
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
	animator->SetNumberAnimations(3);
	
	//Shuriken
	animator->AddKeyFrame(0, { 0,0,13,13 });
	animator->AddKeyFrame(0, { 1 * 13,0,13,13 });
	animator->SetAnimationDelay(0, 100);

	//Spike
	animator->AddKeyFrame(1, { 0,1 * 13,13,13 });
	animator->AddKeyFrame(1, { 1 * 13,1 * 13,13,13 });
	animator->SetAnimationDelay(1, 100);

	//Water
	animator->AddKeyFrame(2, { 0,2 * 13,13,13 });
	animator->AddKeyFrame(2, { 1 * 13,2 * 13,13,13 });
	animator->SetAnimationDelay(2, 100);

	//Boss
	animator->AddKeyFrame(3, { 0,2 * 13,13,13 });
	animator->AddKeyFrame(3, { 1 * 13,2 * 13,13,13 });
	animator->SetAnimationDelay(3, 100);

	animator->SetAnimation(0);
	animator->SetLoop(true);

	// Add physics to the projectile - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	body->body->SetFixedRotation(true);

	b2MassData projectileMass;
	projectileMass.mass = 1.0f;
	projectileMass.center = body->body->GetLocalCenter();
	body->body->SetMassData(&projectileMass);

	body->body->GetFixtureList()[0].SetFriction(0);
	body->body->GetFixtureList()[0].SetSensor(true);

	// Assign projectile class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	body->listener = this;

	body->body->ApplyLinearImpulseToCenter({ direction.x * speed, direction.y * speed }, true);

	return true;
}

bool Projectile::Update(float dt)
{
	b2Vec2 velocity = body->body->GetLinearVelocity();

	b2Transform pbodyPos = body->body->GetTransform();
	position.x = ((float)METERS_TO_PIXELS(pbodyPos.p.x) - width / 2);
	position.y = ((float)METERS_TO_PIXELS(pbodyPos.p.y) - height / 2);

	if (velocity.x < 0)
	{
		animator->LookLeft();
	}
	else
	{
		animator->LookLeft();
	}

	animator->Update();
	animator->Draw((int)position.x, (int)position.y, 0, 0);
	
	if (projectileDeadTimer.ReadSec() > projectileDeadTime)
	{
		Engine::GetInstance().entityManager->DeleteEntity(this);
	}
	
	return true;
}

bool Projectile::CleanUp()
{
	LOG("Cleanup Projectile");
	//Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(body);

	return true;
}

void Projectile::OnCollision(PhysBody* physA, PhysBody* physB)
{
	
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		Engine::GetInstance().entityManager->DeleteEntity(this);
		break;
	case ColliderType::BOULDER:
		Engine::GetInstance().entityManager->DeleteEntity(this);
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM PROJECTILE");
		Engine::GetInstance().entityManager->DeleteEntity(this);
		break;
	case ColliderType::KILL:
		LOG("Collision KILL");
		Engine::GetInstance().entityManager->DeleteEntity(this);
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		Engine::GetInstance().entityManager->DeleteEntity(this);
		break;
	case ColliderType::PROJECTILE_ENEMY:
		LOG("Collision ITEM");
		Engine::GetInstance().entityManager->DeleteEntity(this);
		break;
	case ColliderType::PROJECTILE_PLAYER:
		LOG("Collision ITEM");
		Engine::GetInstance().entityManager->DeleteEntity(this);
		break;
	case ColliderType::PLAYER_ATTACK_LEFT:
		if (Engine::GetInstance().scene.get()->GetPlayer()->IsAttackingLeft())
		{
			Engine::GetInstance().scene.get()->GetPlayer()->SetAttackingLeft(false);
			LOG("Collision KILL");
			Engine::GetInstance().entityManager->DeleteEntity(this);
		}
		break;
	case ColliderType::PLAYER_ATTACK_RIGHT:
		if (Engine::GetInstance().scene.get()->GetPlayer()->IsAttackingRight())
		{
			Engine::GetInstance().scene.get()->GetPlayer()->SetAttackingRight(false);
			LOG("Collision KILL");
			Engine::GetInstance().entityManager->DeleteEntity(this);
		}
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;

	default:
		break;
	}
}

void Projectile::SetAnimation(int id)
{
	animator->SetAnimation(id);
}

void Projectile::SetGravity(float gravity)
{
	body->body->SetGravityScale(gravity);
}

void Projectile::SetCollisionType(int type)
{
	if (type == 0)
	{
		body->ctype = ColliderType::PROJECTILE_PLAYER;
	}
	if (type == 1)
	{
		body->ctype = ColliderType::PROJECTILE_ENEMY;
	}
}
