#include "Bee.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"

Bee::Bee() : Enemy(EntityType::BEE)
{

}

Bee::~Bee()
{
}

bool Bee::Awake()
{
	return true;
}

bool Bee::Start()
{
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	height = parameters.attribute("h").as_int();
	width = parameters.attribute("w").as_int();

	texH = height, texW = width;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), width / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	SetPathfindingType(EnemyType::FLOOR);

	textureName = parameters.attribute("texture").as_string();
	texture = Engine::GetInstance().textures->Load(textureName.c_str());

	animator = new Sprite(texture);
	animator->SetNumberAnimations(4);

	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	height = parameters.attribute("h").as_int();
	width = parameters.attribute("w").as_int();

	// IDLE
	animator->AddKeyFrame(0, { 0,  1 * height,width,height });
	animator->AddKeyFrame(0, { 1 * width, 1 * height,width,height });
	animator->AddKeyFrame(0, { 2 * width, 1 * height,width,height });
	animator->AddKeyFrame(0, { 3 * width, 1 * height,width,height });
	animator->SetAnimationDelay(0, 100);

	// ATTACK
	animator->AddKeyFrame(1, { 0,  0 * height,width,height });
	animator->AddKeyFrame(1, { 1 * width, 0 * height,width,height });
	animator->AddKeyFrame(1, { 2 * width, 0 * height,width,height });
	animator->AddKeyFrame(1, { 3 * width, 0 * height,width,height });
	animator->SetAnimationDelay(0, 100);

	// HIT
	animator->AddKeyFrame(2, { 0,  2 * height,width,height });
	animator->AddKeyFrame(2, { 1 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 2 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 3 * width, 2 * height,width,height });
	animator->SetAnimationDelay(0, 100);
	

	animator->SetAnimation(0);
	animator->SetLoop(true);

	return true;
}

bool Bee::Update(float dt)
{
	animator->Update();
	animator->Draw((int)position.getX(), (int)position.getY(), 0, 0);

	return true;
}

bool Bee::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}
