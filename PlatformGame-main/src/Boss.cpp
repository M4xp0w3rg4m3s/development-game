#include "Boss.h"

#include "Log.h"
#include "Physics.h"
#include "Render.h"
#include "Scene.h"
#include "Engine.h"
#include "Textures.h"
#include "EntityManager.h"
#include "Audio.h"
#include "Projectile.h"

#include <ctime>

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

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY() + height / 4, width / 5, bodyType::DYNAMIC);
	bodyAttackLeft = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX() , (int)position.getY(), 30*2, 30*2, bodyType::DYNAMIC);
	bodyAttackRight = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), 30*2, 30*2, bodyType::DYNAMIC);

	pbody->CreateWeld(bodyAttackLeft, { (float)PIXEL_TO_METERS(width / 4) ,(float)PIXEL_TO_METERS(-20 *2) });
	pbody->CreateWeld(bodyAttackRight, { (float)PIXEL_TO_METERS(-width / 4),(float)PIXEL_TO_METERS(-20 *2) });

	pbody->body->SetFixedRotation(true);
	bodyAttackLeft->body->SetFixedRotation(true);
	bodyAttackRight->body->SetFixedRotation(true);

	//Assign collider type
	pbody->ctype = ColliderType::BOSS;
	bodyAttackLeft->ctype = ColliderType::BOSS_ATTACK_LEFT;
	bodyAttackRight->ctype = ColliderType::BOSS_ATTACK_RIGHT;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool())
	{
		pbody->body->SetGravityScale(100);
		bodyAttackLeft->body->SetGravityScale(100);
		bodyAttackRight->body->SetGravityScale(100);
	}

	pbody->body->GetFixtureList()[0].SetFriction(500.0f);
	bodyAttackLeft->body->GetFixtureList()[0].SetFriction(0);
	bodyAttackLeft->body->GetFixtureList()[0].SetFriction(0);

	//// Add friction and weight
	b2MassData bossMass;
	bossMass.mass = 5.0f;
	bossMass.center = pbody->body->GetLocalCenter();
	pbody->body->SetMassData(&bossMass);

	b2MassData bodyAttackRightMass;
	bodyAttackRightMass.mass = 0.0f;
	bodyAttackRightMass.center = pbody->body->GetLocalCenter();
	bodyAttackRight->body->SetMassData(&bodyAttackRightMass);

	b2MassData bodyAttackLeftMass;
	bodyAttackLeftMass.mass = 0.0f;
	bodyAttackLeftMass.center = pbody->body->GetLocalCenter();
	bodyAttackLeft->body->SetMassData(&bodyAttackLeftMass);

	pbody->listener = this;
	bodyAttackLeft->listener = this;
	bodyAttackRight->listener = this;
	
	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	SetPathfindingType(EnemyType::FLOOR);

	// Texture + Animator
	textureName = parameters.attribute("texture").as_string();
	texture = Engine::GetInstance().textures->Load(textureName.c_str());

	animator = new Sprite(texture);
	animator->SetNumberAnimations(5);

	// IDLE
	animator->AddKeyFrame(0, { 0, 0,width,height });
	animator->AddKeyFrame(0, { 1 * width, 0,width,height });
	animator->AddKeyFrame(0, { 2 * width, 0,width,height });
	animator->AddKeyFrame(0, { 3 * width, 0,width,height });
	animator->AddKeyFrame(0, { 4 * width, 0,width,height });
	animator->AddKeyFrame(0, { 5 * width, 0,width,height });
	animator->AddKeyFrame(0, { 6 * width, 0,width,height });
	animator->AddKeyFrame(0, { 7 * width, 0,width,height });
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

	// ATTACK
	animator->AddKeyFrame(2, { 0,  2 * height,width,height });
	animator->AddKeyFrame(2, { 1 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 1 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 2 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 2 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 3 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 3 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 4 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 4 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 5 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 6 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 7 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 8 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 9 * width, 2 * height,width,height });
	animator->AddKeyFrame(2, { 10 * width, 2 * height,width,height });
	animator->SetAnimationDelay(2, 100);

	// HURT
	animator->AddKeyFrame(3, { 0,  3 * height,width,height });
	animator->AddKeyFrame(3, { 1 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 2 * width, 3 * height,width,height });
	animator->AddKeyFrame(3, { 3 * width, 3 * height,width,height });
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
	animator->AddKeyFrame(4, { 10 * width, 4 * height,width,height });
	animator->AddKeyFrame(4, { 11 * width, 4 * height,width,height });
	animator->SetAnimationDelay(4, 100);

	animator->SetAnimation(0);
	animator->SetLoop(true);

	// Seed the random number generator
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	hitTimer.Start();

	return true;
}

bool Boss::Update(float dt)
{
	animator->Update();

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - (float)texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - (float)texH / 2);

	b2Vec2 linearVelocity = { pbody->body->GetLinearVelocity().x, pbody->body->GetLinearVelocity().y };
	if (fabs(linearVelocity.x) < VELOCITY_THRESHOLD) {
		linearVelocity.x = 0;
	}

	if(Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getX() > 6150)
	{
		// Behaviour
		if (attackTimer.ReadSec() > attackTime)
		{
			isAttacking = true;
		}

		if (lives <= 0)
		{
			if (animator->GetAnimation() != 4)
			{
				animator->SetAnimation(4);
				animator->SetLoop(false);
			}
			if (animator->isAnimFinished())
			{
				Disable();
			}
		}
		else if (isAttacking)
		{
			if (animator->GetAnimation() != 2)
			{
				animator->SetAnimation(2);

				Attack();
			}

			if (animator->GetCurrentFrame_int() == 13)
			{
				if (fallingProjectiles)
				{
					Shoot();
					fallingProjectiles = false;
				}

				isAttacking = false;
				isAttackingLeft = false;
				isAttackingRight = false;

				float randomAttackTime = static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX)) * 2.5f + 0.5f;

				attackTime = randomAttackTime;
				attackTimer.Start();

			}

		}
		else
		{
			Move();
			if (linearVelocity.x > 0 || linearVelocity.x < 0)
			{
				if (animator->GetAnimation() != 1)
				{
					animator->SetAnimation(1);
				}
			}
			else
			{
				pbody->body->SetLinearVelocity({ 0,5 });
				if (animator->GetAnimation() != 0)
				{
					animator->SetAnimation(0);
				}
			}
		}
	}

	//Draw + Flip
	int drawScaleX = (linearVelocity.x < 0 || (linearVelocity.x == 0 && !animator->IsLookingRight())) ? 50 : -50;

	if (linearVelocity.x < 0) {
		animator->LookLeft();
	}
	else if (linearVelocity.x > 0) {
		animator->LookRight();
	}
	else
	{
		b2Vec2 centerPos = { GetCenterPosition().getX(), GetCenterPosition().getY() };

		b2Vec2 directionToPlayer = {
			Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getX() - centerPos.x,
			Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getY() - centerPos.y
		};

		if (linearVelocity.y == 0)
		{
			if (directionToPlayer.x < 0)
			{
				animator->LookLeft();
			}
			else {
				animator->LookRight();
			}
		}
	}
	animator->Draw((int)position.getX(), (int)position.getY(), drawScaleX, -55);

	// Track attack direction
	if (isAttacking) {
		if (drawScaleX == 26) {
			isAttackingLeft = true;
		}
		else {
			isAttackingRight = true;
		}
	}

	return true;
}

bool Boss::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);

	Engine::GetInstance().physics->DeletePhysBody(pbody);
	Engine::GetInstance().physics->DeletePhysBody(bodyAttackLeft);
	Engine::GetInstance().physics->DeletePhysBody(bodyAttackRight);

	return true;
}

void Boss::Attack()
{
	int randAttack = std::rand() % 2;
	
	b2Vec2 centerPos = { GetCenterPosition().getX(), GetCenterPosition().getY() };

	b2Vec2 direction = {
		Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getX() - centerPos.x,
		Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getY() - centerPos.y
	};
	if (randAttack == 1) //Projectiles
	{
		fallingProjectiles = true;
	}
	else //Jump
	{
		if (direction.x > 0)
		{
			pbody->body->ApplyForceToCenter({ 300 * 1.25,-750 *1.25 },true);
			bodyAttackLeft->body->ApplyForceToCenter({300 * 1.25,-750 * 1.25 },true);
			bodyAttackRight->body->ApplyForceToCenter({ 300 * 1.25,-750 * 1.25 },true);
		}
		else if (direction.x < 0)
		{
			pbody->body->ApplyForceToCenter({ -300 * 1.25,-750 * 1.25 }, true);
			bodyAttackLeft->body->ApplyForceToCenter({ -300 * 1.25,-750 * 1.25 }, true);
			bodyAttackRight->body->ApplyForceToCenter({ -300 * 1.25,-750 * 1.25 }, true);
		}
	}
}

void Boss::Shoot()
{
	b2Vec2 direction = { 0,0 };

	b2Vec2 projectilePos = {
		Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getX(),
		40
	};
	b2Vec2 projectilePos1 = {
		Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getX() + (int)(32 * 1.5),
		40
	};
	b2Vec2 projectilePos2 = {
		Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getX() + (32 * 3),
		40
	};
	b2Vec2 projectilePos3 = {
		Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getX() - (int)(32 * 1.5),
		40
	};
	b2Vec2 projectilePos4 = {
		Engine::GetInstance().scene.get()->GetPlayer()->GetCenterPosition().getX() - (32 * 3),
		40
	};

	// Create and initialize the projectiles with its position and direction in world space
	Projectile* projectile = (Projectile*)Engine::GetInstance().entityManager->CreateProjectile(projectilePos, direction, true);
	projectile->SetGravity(1);
	projectile->SetAnimation(3);
	projectile->SetCollisionType(1);

	Projectile* projectile1 = (Projectile*)Engine::GetInstance().entityManager->CreateProjectile(projectilePos1, direction, true);
	projectile1->SetGravity(1);
	projectile1->SetAnimation(3);
	projectile1->SetCollisionType(1);

	Projectile* projectile2 = (Projectile*)Engine::GetInstance().entityManager->CreateProjectile(projectilePos2, direction, true);
	projectile2->SetGravity(1);
	projectile2->SetAnimation(3);
	projectile2->SetCollisionType(1);

	Projectile* projectile3 = (Projectile*)Engine::GetInstance().entityManager->CreateProjectile(projectilePos3, direction, true);
	projectile3->SetGravity(1);
	projectile3->SetAnimation(3);
	projectile3->SetCollisionType(1);

	Projectile* projectile4 = (Projectile*)Engine::GetInstance().entityManager->CreateProjectile(projectilePos4, direction, true);
	projectile4->SetGravity(1);
	projectile4->SetAnimation(3);
	projectile4->SetCollisionType(1);

}

void Boss::Move()
{
	if (pathfinding->resetPathAfterEnd) {
		Vector2D pos = GetPosition();
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)(pos.getX()), (int)(pos.getY()));
		pathfinding->ResetPath(tilePos);
		pathfinding->resetPathAfterEnd = false;
	}
	pathfinding->Compute();

	if (pathfinding->objectiveFound) {
		GoToPath();
	}
	else {
		pbody->body->SetLinearVelocity({ 0,0 });
	}

	if (Engine::GetInstance().scene->debug) {
		// Draw pathfinding 
		pathfinding->DrawPath();
	}
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (physA->ctype == ColliderType::BOSS)
	{
		switch (physB->ctype)
		{
		case ColliderType::PROJECTILE_PLAYER:
			Engine::GetInstance().audio.get()->PlayFx(audioShurikenHitId);
			GetDamaged();
			break;
		case ColliderType::PLAYER_ATTACK_LEFT:
			if (Engine::GetInstance().scene->GetPlayer()->IsAttackingBossLeft())
			{
				GetDamaged();
			}
			break;
		case ColliderType::PLAYER_ATTACK_RIGHT:
			if (Engine::GetInstance().scene->GetPlayer()->IsAttackingBossRight())
			{
				GetDamaged();
			}
			break;
		
		default:
			break;
		}
	}	
	if (physA->ctype == ColliderType::BOSS_ATTACK_LEFT && physB->ctype == ColliderType::PLAYER && isAttackingLeft) {
		Engine::GetInstance().scene->GetPlayer()->SetCanBeAttacked(true);
	}
	else if (physA->ctype == ColliderType::BOSS_ATTACK_RIGHT && physB->ctype == ColliderType::PLAYER && isAttackingRight) {
		Engine::GetInstance().scene->GetPlayer()->SetCanBeAttacked(true);
	}
}

void Boss::GoToPath()
{
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)(pos.getX()), (int)(pos.getY()));

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
		float currentPosX = (float)METERS_TO_PIXELS(pbody->body->GetPosition().x) - (width / 4) - 8;
		float currentPosY = (float)METERS_TO_PIXELS(pbody->body->GetPosition().y) - (width / 4) - 8;

		if (currentPosX != destination.getX()) {
			if (currentPosX < destination.getX()) {
				velocity.x = (float)0.10 * 16;
			}
			else {
				velocity.x = (float)-0.10 * 16;
			}
		}
		else if (currentPosX == destination.getX()) {
			velocity.x = 0;
		}

		if (currentPosY != destination.getY()) {
			if (currentPosY < destination.getY()) {
				velocity.y = (float)0.10 * 16;
			}
			else {
				velocity.y = (float)-0.10 * 16;
			}
		}
		else if (currentPosY == destination.getY()) {
			velocity.y = 0;
		}
		pbody->body->SetLinearVelocity(velocity);
	}
}

bool Boss::IsAttackingLeft()
{
	return isAttackingLeft;
}

bool Boss::IsAttackingRight()
{
	return isAttackingRight;
}

void Boss::GetDamaged()
{
	if (hitTimer.ReadMSec() > hitTime) {
		lives--;
		hitTimer.Start();
	}
}

int Boss::GetLives() const
{
	return lives;
}
