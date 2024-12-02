#include "Octopus.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"
#include "Projectile.h"

Octopus::Octopus() : Enemy(EntityType::OCTOPUS)
{

}

Octopus::~Octopus()
{
}

bool Octopus::Awake()
{
	return true;
}

bool Octopus::Start()
{
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	height = parameters.attribute("h").as_int();
	width = parameters.attribute("w").as_int();

	texH = height*2, texW = width*2;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), width / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	SetPathfindingType(EnemyType::WATER);

	textureName = parameters.attribute("texture").as_string();
	texture = Engine::GetInstance().textures->Load(textureName.c_str());

	animator = new Sprite(texture);
	animator->SetNumberAnimations(1);

	// IDLE
	animator->AddKeyFrame(0, { 0, 5 * texH,texW,texH });
	animator->AddKeyFrame(0, { 1 * texW, 5 * texH,texW,texH });
	animator->AddKeyFrame(0, { 2 * texW, 5 * texH,texW,texH });
	animator->AddKeyFrame(0, { 3 * texW, 5 * texH,texW,texH });
	animator->SetAnimationDelay(0, 100);

	animator->SetAnimation(0);
	animator->SetLoop(true);

	return true;
}

bool Octopus::Update(float dt)
{

	//Add a physics to an item - update the position of the object from the physics.  
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - width / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - height / 2);

	if (pathfinding->resetPathAfterEnd) {
		Vector2D pos = GetPosition();
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
		pathfinding->ResetPath(tilePos);
		pathfinding->resetPathAfterEnd = false;
	}
	pathfinding->Compute();

	if (pathfinding->objectiveFound) {
		GoToPath();
	}

	// Activate or deactivate debug mode
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	if (debug) {
		// Draw pathfinding 
		pathfinding->DrawPath();
	}

	if (attackTimer.ReadSec() >= attackTime)
	{
		Shoot();
		attackTimer.Start();
	}

	animator->Update();
	if (pbody->body->GetLinearVelocity().x < 0) {
		animator->LookLeft();
		animator->Draw((int)position.getX(), (int)position.getY(), 28, -16);
	}
	else {
		animator->LookRight();
		animator->Draw((int)position.getX(), (int)position.getY(), -4, -16);
	}
	
	return true;
}

bool Octopus::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

void Octopus::Shoot()
{
	b2Vec2 direction = { 0,-1 };

	b2Vec2 centerPos = { GetCenterPosition().getX(), GetCenterPosition().getY() };

	// Define the projectile spawn position with an offset in the normalized direction
	float offset = static_cast<float>(height); // Offset distance to avoid overlapping with the player
	b2Vec2 projectilePos = {
		centerPos.x + direction.x * offset,
		centerPos.y + direction.y * offset
	};


	// Create and initialize the projectile with its position and direction in world space
	Projectile* projectile = (Projectile*)Engine::GetInstance().entityManager->CreateProjectile(projectilePos, direction, true);
	projectile->SetGravity(3);
	projectile->SetAnimation(2);
	// Reset the attack timer to manage firing rate
	attackTimer.Start();
}

void Octopus::GoToPath()
{
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());

	Vector2D destination = { NULL, NULL };

	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().x);

	int index = 0;
	for (const auto& tile : pathfinding->pathTiles) {
		if (tilePos.getX() == tile.getX()) {
			float destinationX = NULL;
			float destinationY = NULL;
			if (index == 0) {
				destinationX = pathfinding->pathTiles[index].getX();
				destinationY = pathfinding->pathTiles[index].getY();
			}
			else {
				destinationX = pathfinding->pathTiles[index - 1].getX();
				destinationY = pathfinding->pathTiles[index - 1].getY();
			}
			destination = Engine::GetInstance().map.get()->MapToWorld((int)destinationX, (int)tilePos.getY());
			break;
		}
		index++;
	}

	if (destination.getX() != NULL && destination.getY() != NULL) {
		float currentPosX = METERS_TO_PIXELS(pbody->body->GetPosition().x) - width / 2;

		if (currentPosX != destination.getX()) {
			if (currentPosX < destination.getX()) {
				velocity.x = 0.05 * 16;
			}
			else {
				velocity.x = -0.05 * 16;
			}
		}
		else if(currentPosX == destination.getX()){
			velocity.x = 0;
		}
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
	}
}
