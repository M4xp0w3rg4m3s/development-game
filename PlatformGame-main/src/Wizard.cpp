#include "Wizard.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"

Wizard::Wizard() : Enemy(EntityType::WIZARD)
{

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), width / 2, bodyType::DYNAMIC);

	texW = width, texH = height;

}

Wizard::~Wizard()
{
}

bool Wizard::Awake()
{
	return true;
}

bool Wizard::Start()
{
	textureName = parameters.attribute("texture").as_string();
	texture = Engine::GetInstance().textures->Load(textureName.c_str());

	animator = new Sprite(texture);
	animator->SetNumberAnimations(4);

	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	height = parameters.attribute("h").as_int();
	width = parameters.attribute("w").as_int();

	// IDLE
	animator->AddKeyFrame(0, { 0, 0,width,height });
	animator->AddKeyFrame(0, { 1 * width, 0,width,height });
	animator->SetAnimationDelay(0, 100);

	// WALK
	animator->AddKeyFrame(1, { 0, 1 * height,width,height });
	animator->AddKeyFrame(1, { 1 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 2 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 3 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 4 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 5 * width, 1 * height,width,height });
	animator->SetAnimationDelay(0, 100);

	// SLEEPING
	animator->AddKeyFrame(2, { 0, 2 * height,width,height });
	animator->AddKeyFrame(2, { 1 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 2 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 3 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 4 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 5 * width, 2 * height,width,height });
	animator->SetAnimationDelay(0, 100);

	// GOING TO SLEEP
	animator->AddKeyFrame(2, { 0, 3 * height,width,height });
	animator->AddKeyFrame(2, { 1 * width, 3 * height,width,height });
	animator->AddKeyFrame(2, { 2 * width, 3 * height,width,height });
	animator->AddKeyFrame(2, { 3 * width, 3 * height,width,height });
	animator->AddKeyFrame(2, { 4 * width, 3 * height,width,height });
	animator->AddKeyFrame(2, { 5 * width, 3 * height,width,height });
	animator->SetAnimationDelay(0, 100);

	animator->SetAnimation(0);
	animator->SetLoop(true);

	texH = height, texW = width;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	return true;
}

bool Wizard::Update(float dt)
{
	animator->Update();
	animator->Draw((int)position.getX(), (int)position.getY(), 0, 0);

	return true;
}

bool Wizard::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}
