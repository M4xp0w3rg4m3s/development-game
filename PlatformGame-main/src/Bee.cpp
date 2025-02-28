#include "Bee.h"
#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"
#include "Audio.h"

Bee::Bee() : Enemy(EntityType::BEE)
{
	audioShurikenHitId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/shurikenHit.wav");
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
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	position.setX(parameters.attribute("x").as_float());
	position.setY(parameters.attribute("y").as_float());
	height = parameters.attribute("h").as_int();
	width = parameters.attribute("w").as_int();
	enemyId = parameters.attribute("id").as_string();

	texH = height, texW = width;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), width / 5, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	pbody->listener = this;

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	SetPathfindingType(EnemyType::AIR);

	textureName = parameters.attribute("texture").as_string();
	texture = Engine::GetInstance().textures->Load(textureName.c_str());

	animator = new Sprite(texture);
	animator->SetNumberAnimations(4);

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
	//Add a physics to an item - update the position of the object from the physics.  
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX((float)(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2));
	position.setY((float)(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2));

	if (pathfinding->resetPathAfterEnd) {
		Vector2D pos = GetPosition();
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)pos.getX(), (int)pos.getY());
		pathfinding->ResetPath(tilePos);
		pathfinding->resetPathAfterEnd = false;
	}
	pathfinding->Compute();

	if (pathfinding->objectiveFound) {
		GoToPath();
	}
	else {
		pbody->body->SetLinearVelocity({0,0});
	}

	if (Engine::GetInstance().scene->debug) {
		// Draw pathfinding 
		pathfinding->DrawPath();
	}

	if (pbody->body->GetLinearVelocity().x > 0) {
		animator->LookLeft();
	}
	else {
		animator->LookRight();
	}

	animator->Update();
	animator->Draw((int)position.getX(), (int)position.getY(), 12, -5);

	return true;
}

bool Bee::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

void Bee::OnCollision(PhysBody* physA, PhysBody* physB)
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

void Bee::GoToPath()
{
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)pos.getX(), (int)pos.getY());

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
		float currentPosX = (float)METERS_TO_PIXELS(pbody->body->GetPosition().x) - (width / 5)*2;
		float currentPosY = (float)METERS_TO_PIXELS(pbody->body->GetPosition().y) - (width / 5)*2;

		if (currentPosX != destination.getX()) {
			if (currentPosX < destination.getX()) {
				velocity.x = (float)0.13 * 16;
			}
			else {
				velocity.x = (float)-0.13 * 16;
			}
		}
		else if (currentPosX == destination.getX()) {
			velocity.x = 0;
		}

		if (currentPosY != destination.getY()) {
			if (currentPosY < destination.getY()) {
				velocity.y = (float)0.13 * 16;
			}
			else {
				velocity.y = (float)-0.13 * 16;
			}
		}
		else if (currentPosY == destination.getY()) {
			velocity.y = 0;
		}
		pbody->body->SetLinearVelocity(velocity);
	}
}
