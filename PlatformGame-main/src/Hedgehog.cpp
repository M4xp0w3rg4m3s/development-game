#include "Hedgehog.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"
#include "Projectile.h"
#include "Player.h"
#include "Audio.h"

Hedgehog::Hedgehog() : Enemy(EntityType::HEDGEHOG)
{
	audioShurikenHitId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/shurikenHit.wav");
}

Hedgehog::~Hedgehog()
{
}

bool Hedgehog::Awake()
{
	return true;
}

bool Hedgehog::Start()
{
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	height = parameters.attribute("h").as_int();
	width = parameters.attribute("w").as_int();
	enemyId = parameters.attribute("id").as_string();

	texH = height, texW = width;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), width / 3, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	pbody->listener = this;

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
	animator->LookLeft();
	return true;
}

bool Hedgehog::Update(float dt)
{
	//Add a physics to an item - update the position of the object from the physics.  
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

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

	if (pbody->body->GetLinearVelocity().x < 0) {
		animator->LookLeft();
	}
	else {
		animator->LookRight();
	}

	animator->Update();
	animator->Draw((int)position.getX(), (int)position.getY(), 0, -3);

	return true;
}

bool Hedgehog::CleanUp()
{
	LOG("Cleanup Hedgehog");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}
void Hedgehog::Shoot()
{
	b2Vec2 direction = { 0,0 };

	if (animator->GetPlayerDir() == RIGHT)
	{
		direction = { 1,0 };
	}
	else
	{
		direction = { -1,0 };
	}

	b2Vec2 centerPos = { GetCenterPosition().getX(), GetCenterPosition().getY() };

	// Define the projectile spawn position with an offset in the normalized direction
	float offset = static_cast<float>(height); // Offset distance to avoid overlapping with the player
	b2Vec2 projectilePos = {
		centerPos.x + direction.x * offset,
		centerPos.y + direction.y * offset
	};


	// Create and initialize the projectile with its position and direction in world space
	Projectile* projectile = (Projectile*)Engine::GetInstance().entityManager->CreateProjectile(projectilePos, direction, true);
	projectile->SetAnimation(1);
	projectile->SetGravity(0.1);
	projectile->SetCollisionType(1);

	// Reset the attack timer to manage firing rate
	attackTimer.Start();
}

void Hedgehog::GoToPath()
{
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());

	Vector2D destination = { NULL, NULL };

	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().x);

	int index = 0;
	for (const auto& tile : pathfinding->pathTiles) {
		if (tilePos == tile) {
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
			destination = Engine::GetInstance().map.get()->MapToWorld((int)destinationX, (int)destinationY);
			break;
		}
		index++;
	}

	if (destination.getX() != NULL && destination.getY() != NULL) {
		float currentPosX = METERS_TO_PIXELS(pbody->body->GetPosition().x) - (width / 4) - 8;
		float currentPosY = METERS_TO_PIXELS(pbody->body->GetPosition().y) - (width / 4) - 8;

		if (currentPosX != destination.getX()) {
			if (currentPosX < destination.getX()) {
				velocity.x = 0.10 * 16;
			}
			else {
				velocity.x = -0.10 * 16;
			}
		}
		else if (currentPosX == destination.getX()) {
			velocity.x = 0;
		}

		if (currentPosY != destination.getY()) {
			if (currentPosY < destination.getY()) {
				velocity.y = 0.10 * 16;
			}
			else {
				velocity.y = -0.10 * 16;
			}
		}
		else if (currentPosY == destination.getY()) {
			velocity.y = 0;
		}
		pbody->body->SetLinearVelocity(velocity);
	}
}
void Hedgehog::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PROJECTILE_PLAYER:
		Engine::GetInstance().audio.get()->PlayFx(audioShurikenHitId);
		Disable();
		break;
	default:
		break;
	}
}
