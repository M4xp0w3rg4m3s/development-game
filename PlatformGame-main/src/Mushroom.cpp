#include "Mushroom.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"

Mushroom::Mushroom() : Enemy(EntityType::MUSHROOM)
{
	/*texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Enemies/Mushroom.png");
	animator = new Sprite(texture);*/

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), width / 2, bodyType::DYNAMIC);

	texW = width, texH = height;

	this->position = position;

}

Mushroom::~Mushroom()
{
}

bool Mushroom::Awake()
{
	return true;
}

bool Mushroom::Start()
{
	textureName = parameters.attribute("texture").as_string();
	texture = Engine::GetInstance().textures->Load(textureName.c_str());

	animator = new Sprite(texture);
	animator->SetNumberAnimations(1);

	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	height = parameters.attribute("h").as_int();
	width = parameters.attribute("w").as_int();

	// IDLE
	animator->AddKeyFrame(0, { 0, 0,width,height });
	animator->AddKeyFrame(0, { 1 * width, 0,width,height });
	animator->AddKeyFrame(0, { 2 * width, 0,width,height });
	animator->AddKeyFrame(0, { 3 * width, 0,width,height });
	animator->AddKeyFrame(0, { 4 * width, 0,width,height });
	animator->AddKeyFrame(0, { 5 * width, 0,width,height });
	animator->AddKeyFrame(0, { 6 * width, 0,width,height });
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

bool Mushroom::Update(float dt)
{
	
	return true;
}

bool Mushroom::CleanUp()
{
	return true;
}
