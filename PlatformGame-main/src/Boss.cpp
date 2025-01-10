#include "Boss.h"

#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"
#include "Audio.h"

Boss::Boss() : Enemy(EntityType::BOSS)
{
	audioShurikenHitId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/shurikenHit.wav");
}

Boss::~Boss()
{
}

bool Boss::Awake()
{
	return true;
}

bool Boss::Start()
{

	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	position.setX(parameters.attribute("x").as_float());
	position.setY(parameters.attribute("y").as_float());
	height = parameters.attribute("h").as_int();
	width = parameters.attribute("w").as_int();
	enemyId = parameters.attribute("id").as_string();

	texH = height, texW = width;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY() + height / 4, width / 4, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Assign projectile class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	//// Initialize pathfinding
	//pathfinding = new Pathfinding();
	//ResetPath();

	//SetPathfindingType(EnemyType::FLOOR);

	textureName = parameters.attribute("texture").as_string();
	texture = Engine::GetInstance().textures->Load(textureName.c_str());

	animator = new Sprite(texture);
	animator->SetNumberAnimations(6);

	// IDLE
	animator->AddKeyFrame(0, { 0, 0,width,height });
	animator->AddKeyFrame(0, { 1 * width, 0,width,height });
	animator->AddKeyFrame(0, { 2 * width, 0,width,height });
	animator->AddKeyFrame(0, { 3 * width, 0,width,height });
	animator->AddKeyFrame(0, { 4 * width, 0,width,height });
	animator->AddKeyFrame(0, { 5 * width, 0,width,height });
	animator->SetAnimationDelay(0, 100);

	// WALK
	animator->AddKeyFrame(1, { 0,  1 * height,width,height });
	animator->AddKeyFrame(1, { 1 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 2 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 3 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 4 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 5 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 6 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 7 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 8 * width, 1 * height,width,height });
	animator->AddKeyFrame(1, { 9 * width, 1 * height,width,height });
	animator->SetAnimationDelay(1, 100);

	// UP
	animator->AddKeyFrame(2, { 0,  2 * height,width,height });
	animator->AddKeyFrame(2, { 1 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 2 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 3 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 4 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 5 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 6 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 7 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 8 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 9 * width, 2 * height,width,height });
	animator->SetAnimationDelay(2, 100);

	// DOWN
	animator->AddKeyFrame(3, { 0,  3 * height,width,height });
	animator->AddKeyFrame(3, { 1 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 2 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 3 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 4 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 5 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 6 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 7 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 8 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 9 * width, 3 * height,width,height });
	animator->SetAnimationDelay(3, 100);

	// DIE
	animator->AddKeyFrame(4, { 0,  4 * height,width,height });
	animator->AddKeyFrame(4, { 1 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 2 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 3 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 4 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 5 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 6 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 7 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 8 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 9 * width, 4 * height,width,height });
	animator->SetAnimationDelay(4, 100);

	// HIT
	animator->AddKeyFrame(5, { 0,  5 * height,width,height });
	animator->AddKeyFrame(5, { 1 * width, 5 * height,width,height });
	animator->AddKeyFrame(5, { 2 * width, 5 * height,width,height });
	animator->AddKeyFrame(5, { 3 * width, 5 * height,width,height });

	animator->SetAnimation(0);
	animator->SetLoop(true);

	return true;
}

bool Boss::Update(float dt)
{
	// Animation Selection
	if (lives <= 0)
	{
		animator->SetAnimation(4);
		if (animator->isAnimFinished())
		{
			Disable();
		}
	}
	else if (IsAttacking)
	{
		if (IsUp)
		{
			//Projectile attack
		}
		else 
		{
			//melee attack
		}
	}
	else
	{
		if (pbody->body->GetLinearVelocity().y < 0)
		{
			animator->SetAnimation(2);
		}
		else if (pbody->body->GetLinearVelocity().y > 0)
		{
			animator->SetAnimation(3);
		}
		else if (pbody->body->GetLinearVelocity().x > 0 || pbody->body->GetLinearVelocity().x < 0)
		{
			animator->SetAnimation(1);
		}
		else
		{
			animator->SetAnimation(0);
		}
	}

	// Behaviour
	
	////Add a physics to an item - update the position of the object from the physics.  
	//b2Transform pbodyPos = pbody->body->GetTransform();
	//position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - (float)texH / 2);
	//position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - (float)texH / 2);

	//if (pathfinding->resetPathAfterEnd) {
	//	Vector2D pos = GetPosition();
	//	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)(pos.getX()), (int)(pos.getY()));
	//	pathfinding->ResetPath(tilePos);
	//	pathfinding->resetPathAfterEnd = false;
	//}
	//pathfinding->Compute();

	//if (pathfinding->objectiveFound) {
	//	GoToPath();
	//}

	//if (Engine::GetInstance().scene->debug) {
	//	// Draw pathfinding 
	//	pathfinding->DrawPath();
	//}
	//else {
	//	pbody->body->SetLinearVelocity({ 0,0 });
	//}

	animator->Update();

	//Draw + Flip
	if (pbody->body->GetLinearVelocity().x > 0) {
		animator->LookLeft();
		animator->Draw((int)position.getX(), (int)position.getY(), -width / 2, -5);
	}
	else if(pbody->body->GetLinearVelocity().x < 0) {
		animator->LookRight();
		animator->Draw((int)position.getX(), (int)position.getY(), -width / 2, -5);
	}
	else
	{
		animator->Draw((int)position.getX(), (int)position.getY(), -width / 2, -5);
	}
	return true;
}

bool Boss::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PROJECTILE_PLAYER:
		Engine::GetInstance().audio.get()->PlayFx(audioShurikenHitId);
		lives--;
		break;
	default:
		break;
	}
}

//void Boss::GoToPath()
//{
//	Vector2D pos = GetPosition();
//	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)(pos.getX()), (int)(pos.getY()));
//
//	Vector2D destination = { NULL, NULL };
//
//	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().x);
//
//	int index = 0;
//	for (const auto& tile : pathfinding->pathTiles) {
//		if (tilePos == tile) {
//			float destinationX = NULL;
//			float destinationY = NULL;
//			if (index == 0) {
//				destinationX = pathfinding->pathTiles[index].getX();
//				destinationY = pathfinding->pathTiles[index].getY();
//			}
//			else {
//				destinationX = pathfinding->pathTiles[index - 1].getX();
//				destinationY = pathfinding->pathTiles[index - 1].getY();
//			}
//			destination = Engine::GetInstance().map.get()->MapToWorld((int)destinationX, (int)destinationY);
//			break;
//		}
//		index++;
//	}
//
//	if (destination.getX() != NULL && destination.getY() != NULL) {
//		float currentPosX = (float)METERS_TO_PIXELS(pbody->body->GetPosition().x) - (width / 4) - 8;
//		float currentPosY = (float)METERS_TO_PIXELS(pbody->body->GetPosition().y) - (width / 4) - 8;
//
//		if (currentPosX != destination.getX()) {
//			if (currentPosX < destination.getX()) {
//				velocity.x = (float)0.10 * 16;
//			}
//			else {
//				velocity.x = (float)-0.10 * 16;
//			}
//		}
//		else if (currentPosX == destination.getX()) {
//			velocity.x = 0;
//		}
//
//		if (currentPosY != destination.getY()) {
//			if (currentPosY < destination.getY()) {
//				velocity.y = (float)0.10 * 16;
//			}
//			else {
//				velocity.y = (float)-0.10 * 16;
//			}
//		}
//		else if (currentPosY == destination.getY()) {
//			velocity.y = 0;
//		}
//		pbody->body->SetLinearVelocity(velocity);
//	}
//}
