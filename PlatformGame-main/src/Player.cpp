#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Projectile.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";

	audioPlayerStepsId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/steps3.wav"); //AUDIO STEPS
	
}

Player::~Player() {
	delete animator;
}

bool Player::Awake() {

	//Initialize Player parameters
	position = Vector2D(192, 384);
	return true;
}

bool Player::Start() {

	texture = Engine::GetInstance().textures->Load(textureName.c_str());
	animator = new Sprite(texture);

	animator->SetNumberAnimations(8);
	// RUN
	animator->AddKeyFrame(0, { 0 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 1 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 2 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 3 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 4 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 5 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 6 * 64,1 * 64,64,64 });
	animator->AddKeyFrame(0, { 7 * 64,1 * 64,64,64 });
	animator->SetAnimationDelay(0,100);

	// IDLE
	animator->AddKeyFrame(1, { 0 * 64,0 * 64,64,64 });
	animator->AddKeyFrame(1, { 1 * 64,0 * 64,64,64 });
	animator->AddKeyFrame(1, { 2 * 64,0 * 64,64,64 });
	animator->AddKeyFrame(1, { 3 * 64,0 * 64,64,64 });
	animator->SetAnimationDelay(1, 100);

	//FALL
	animator->AddKeyFrame(2, { 0 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(2, { 1 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(2, { 2 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(2, { 3 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(2, { 4 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(2, { 5 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(2, { 6 * 64,3 * 64,64,64 });
	animator->SetAnimationDelay(2, 100);

	//DIE
	animator->AddKeyFrame(3, { 0 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(3, { 1 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(3, { 2 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(3, { 3 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(3, { 4 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(3, { 5 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(3, { 6 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(3, { 7 * 64,5 * 64,64,64 });
	animator->SetAnimationDelay(3, 100);

	//COMBO PART1
	animator->AddKeyFrame(4, { 0 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(4, { 1 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(4, { 2 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(4, { 3 * 64,6 * 64,64,64 });
	animator->SetAnimationDelay(4, 100);

	//COMBO PART2
	animator->AddKeyFrame(5, { 4 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(5, { 5 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(5, { 6 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(5, { 7 * 64,6 * 64,64,64 });
	animator->SetAnimationDelay(5, 100);

	animator->SetAnimation(0);



	// Add physics to the player - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	body = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), width, height, bodyType::DYNAMIC);
	bodyBot = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), width * 0.8, height * 0.25, bodyType::DYNAMIC);
	bodyAttackLeft = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), width * 1.5, height * 1.5, bodyType::DYNAMIC);
	bodyAttackRight = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), width * 1.5, height * 1.5, bodyType::DYNAMIC);

	body->CreateWeld(bodyBot, { 0,(float)PIXEL_TO_METERS((-height / 2)) });
	body->CreateWeld(bodyAttackLeft, { (float)PIXEL_TO_METERS((width * 1.5)) ,(float)PIXEL_TO_METERS((height / 4)) });
	body->CreateWeld(bodyAttackRight, { (float)PIXEL_TO_METERS((-width * 1.5)),(float)PIXEL_TO_METERS((height / 4)) });

	body->body->SetFixedRotation(true);
	bodyBot->body->SetFixedRotation(true);
	bodyAttackLeft->body->SetFixedRotation(true);
	bodyAttackRight->body->SetFixedRotation(true);

	b2MassData playerMass;
	playerMass.mass = 2.3f;
	playerMass.center = body->body->GetLocalCenter();
	body->body->SetMassData(&playerMass);

	body->body->GetFixtureList()[0].SetFriction(0);

	// Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	body->listener = this;
	bodyBot->listener = this;
	bodyAttackLeft->listener = this;
	bodyAttackRight->listener = this;

	// Assign collider type
	body->ctype = ColliderType::PLAYER;
	bodyBot->ctype = ColliderType::GROUND_CHECK;
	bodyAttackLeft->ctype = ColliderType::PLAYER_ATTACK_LEFT;
	bodyAttackRight->ctype = ColliderType::PLAYER_ATTACK_RIGHT;

	/*b2Filter filter;
	filter.categoryBits = Engine::GetInstance().physics.get()->playerAttackLayer;
	filter.maskBits = Engine::GetInstance().physics.get()->EnemyLayer;

	bodyAttackLeft->body->GetFixtureList()[0].SetFilterData(filter);
	bodyAttackRight->body->GetFixtureList()[0].SetFilterData(filter);*/
	
	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, body->body->GetLinearVelocity().y);

	bool wasGrounded = isGrounded;

	isGrounded = bodyBot->activeCollisions != 0;

	//Jump SoundFX
	if (isGrounded && !wasGrounded) {
		Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId);
	}
	

	if (!godMode) {
		if (state != PlayerState::DYING && state != PlayerState::DEAD)
		{
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocity.x = -0.2 * 16;
				animator->LookLeft();
				if ((state == PlayerState::WOMBO || state == PlayerState::COMBO) && animator->GetPlayerDir() == RIGHT)
				{
					state = PlayerState::RUNNING;
				}
				
			}
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocity.x = 0.2 * 16;
				animator->LookRight();
				if ((state == PlayerState::WOMBO || state == PlayerState::COMBO) && animator->GetPlayerDir() == LEFT)
				{
					state = PlayerState::RUNNING;
				}
			}
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
				if (isGrounded) {
					velocity.y = 0;
					body->body->SetLinearVelocity(velocity);
					body->body->ApplyLinearImpulseToCenter(b2Vec2{ 0, -7.5}, true);
					bodyBot->body->ApplyLinearImpulseToCenter(b2Vec2{ 0, -7.5}, true);
					bodyAttackLeft->body->ApplyLinearImpulseToCenter(b2Vec2{ 0, -7.5 }, true);
					bodyAttackRight->body->ApplyLinearImpulseToCenter(b2Vec2{ 0, -7.5 }, true);
					isGrounded = false;
					state = PlayerState::JUMPING;
				}
			}
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_REPEAT) {
		
			state = PlayerState::DYING;
		}
	}
	else {
		body->body->SetGravityScale(0);
		bodyBot->body->SetGravityScale(0);
		bodyAttackLeft->body->SetGravityScale(0);
		bodyAttackRight->body->SetGravityScale(0);

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -0.3 * 16;
			animator->LookLeft();
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = 0.3 * 16;
			animator->LookRight();
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			velocity.y = -0.3 * 16;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			velocity.y = 0.3 * 16;
		}
		else {
			velocity.y = 0;
		}

	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (!godMode) {
			godMode = true;
		}
		else {
			godMode = false;
			body->body->SetGravityScale(1);
			bodyBot->body->SetGravityScale(1);
			bodyAttackLeft->body->SetGravityScale(1);
			bodyAttackRight->body->SetGravityScale(1);
		}
	}

	body->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = body->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - 64 / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - 64 / 2);


	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_P) == KEY_DOWN && attackShurikenTimer.ReadMSec() > attackShurikenTime) {
		Shoot();
	}

	if (state != PlayerState::DYING && state != PlayerState::DEAD){
		if (state == PlayerState::WOMBO) {
			if (animator->isAnimFinished() && animator->GetAnimation() != 5) {
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && attackReactionTimer.ReadMSec() < reactionTimeMs) {
					animator->SetAnimation(5);
					animator->SetLoop(false);
					state = PlayerState::COMBO;
				}
				else if (attackReactionTimer.ReadMSec() > reactionTimeMs) {
					animator->SetAnimation(1);
					state = PlayerState::IDLE;
				}
				attackCooldownTimer.Start();
			}
		}
		else if (state == PlayerState::COMBO) {
			if (animator->GetCurrentFrame_int() == 3) {
				SDL_Delay(100);
				state = PlayerState::IDLE;
			}
		}
		else {
			bool keyQPressed = Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_DOWN;
			bool canAttack = attackCooldownTimer.ReadMSec() > attackCooldown;

			if (keyQPressed && canAttack) {
				if (enemyHasEntered)
				{
					if (animator->IsLookingLeft() && isAttackingLeft)
					{
						Engine::GetInstance().entityManager->DeleteEntity(enemyAttacked);
					}
					else if (animator->IsLookingRight() && isAttackingRight)
					{
						Engine::GetInstance().entityManager->DeleteEntity(enemyAttacked);
					}
					isAttackingRight = false;
					isAttackingLeft = false;
					enemyHasEntered = false;
				}
				
				if (animator->GetAnimation() != 4) {
					if (animator->IsLookingLeft())
					{
						bodyAttackLeft->body->SetEnabled(true);
					}
					else
					{
						bodyAttackRight->body->SetEnabled(true);
					}
					animator->SetAnimation(4);
					animator->SetLoop(false);
					
					state = PlayerState::WOMBO;
					attackReactionTimer.Start();
				}
			}
			else {
				if (isGrounded) {
					if (velocity.x == 0) {
						if (animator->GetAnimation() != 1) {
							animator->SetAnimation(1);
							state = PlayerState::IDLE;
						}
					}
					else {
						if (animator->GetAnimation() != 0) {
							animator->SetAnimation(0);
							state = PlayerState::RUNNING;
						}
					}
				}
				else {
					if (animator->GetAnimation() != 2) {
						animator->SetAnimation(2);
						animator->SetLoop(false);
						state = PlayerState::JUMPING;
					}
				}
			}
		}
	}
	else if (state == PlayerState::DYING)
	{
		lives = 100;
		if (animator->GetAnimation() != 3) {
			animator->SetAnimation(3);
			animator->SetLoop(false);
		}	
		else if (animator->isAnimFinished())
		{
			state = PlayerState::DEAD;
			deadTimer.Start();
		}
	}
	else if (state == PlayerState::DEAD) {
		if (deadTimer.ReadSec() == deadTime) {
			ResetPlayer();
		}
	}

	//Walking SoundFX
	if (state == PlayerState::RUNNING && (animator->GetAnimation() == 0) && (animator->GetCurrentFrame_int() == 2 && animator->GetLastFrame_int() != 2 || animator->GetCurrentFrame_int() == 7 && animator->GetLastFrame_int() != 7)) {
		Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId);	//player steps audio updates every step	
	}

	animator->Update();
	switch (state)
	{
	case IDLE:
		animator->Draw((int)position.getX(), (int)position.getY(), 0, -7);
		break;
	case RUNNING:
		animator->Draw((int)position.getX(), (int)position.getY(), 0, -4);
		break;
	case JUMPING:
		animator->Draw((int)position.getX(), (int)position.getY(), 0, 1);
		break;
	case WOMBO:
		animator->Draw((int)position.getX(), (int)position.getY(), 0, -7);
		break;
	case COMBO:
		animator->Draw((int)position.getX(), (int)position.getY(), 0, -7);
		break;
	case DYING:
		animator->Draw((int)position.getX(), (int)position.getY(), 6, 8);
		break;
	case DEAD:
		animator->Draw((int)position.getX(), (int)position.getY(), 6, 8);
		break;
	default:
		animator->Draw((int)position.getX(), (int)position.getY());
		break;
	}

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (physA->ctype == ColliderType::GROUND_CHECK) {
		if (physB->ctype == ColliderType::KILL) {
			KillPlayer();
		}
	}
	if (physA->ctype == ColliderType::PLAYER_ATTACK_LEFT && physB->ctype == ColliderType::ENEMY) {
		isAttackingLeft = true;
		enemyHasEntered = true;
		enemyAttacked = physB->listener;
	}
	else if (physA->ctype == ColliderType::PLAYER_ATTACK_RIGHT && physB->ctype == ColliderType::ENEMY) {
		isAttackingRight = true;
		enemyHasEntered = true;
		enemyAttacked = physB->listener;
	}
	if (physA->ctype != ColliderType::PLAYER_ATTACK_LEFT && physA->ctype != ColliderType::PLAYER_ATTACK_RIGHT){
		switch (physB->ctype)
		{
		case ColliderType::KILL:
			LOG("Collision KILL");
			break;
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			//Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);			//AUDIO PEPE
			//Engine::GetInstance().physics.get()->DeletePhysBody(physB);
			break;
		case ColliderType::ENEMY:
			LOG("Collision UNKNOWN");
			body->body->ApplyLinearImpulseToCenter({ 0,-1 }, true);

			if (hitTimer.ReadMSec() > hitTime)
			{
				lives--;
				if (lives <= 0)
				{
					KillPlayer();
				}
				hitTimer.Start();
			}
			break;
		case ColliderType::PROJECTILE_ENEMY:
			LOG("Collision Projectile");
			body->body->ApplyLinearImpulseToCenter({ 0,-1 }, true);

			if (hitTimer.ReadMSec() > hitTime)
			{
				lives--;
				if (lives <= 0)
				{
					KillPlayer();
				}
				hitTimer.Start();
			}
			break;
		case ColliderType::UNKNOWN:
			LOG("Collision UNKNOWN");
			break;
		default:
			break;
		}
	}
}

void Player::KillPlayer()
{
	if(!godMode)	state = PlayerState::DYING;
}

void Player::ResetPlayer()
{
	Disable();
	Engine::GetInstance().physics->DeletePhysBody(body);
	Engine::GetInstance().physics->DeletePhysBody(bodyBot);
	Engine::GetInstance().physics->DeletePhysBody(bodyAttackLeft);
	Engine::GetInstance().physics->DeletePhysBody(bodyAttackRight);
	position = Vector2D(192, 384);
	Engine::GetInstance().scene->CameraReset();
	state = PlayerState::IDLE;
	lives = 3;
	Enable();
}

void Player::ResetPlayer(int level)
{
	Disable();
	Engine::GetInstance().physics->DeletePhysBody(body);
	Engine::GetInstance().physics->DeletePhysBody(bodyBot);
	Engine::GetInstance().physics->DeletePhysBody(bodyAttackLeft);
	Engine::GetInstance().physics->DeletePhysBody(bodyAttackRight);
	if (level == 1) {
		position = Vector2D(192, 384);
	}
	if (level == 2) {
		position = Vector2D(192, 320);
	}
	Engine::GetInstance().scene->CameraReset();
	state = PlayerState::IDLE;
	lives = 3;
	Enable();
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	body->body->SetTransform(bodyPos, 0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = body->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

Vector2D Player::GetCenterPosition()
{
	// Get the body's position in meters
	b2Vec2 bodyPos = body->body->GetTransform().p;

	// Convert width and height to meters for accurate calculations
	float halfWidthInMeters = PIXEL_TO_METERS(width) / 2.0f;
	float halfHeightInMeters = PIXEL_TO_METERS(height) / 2.0f;

	// Calculate the center position in meters
	b2Vec2 centerPosInMeters = { bodyPos.x , bodyPos.y };

	// Convert center position back to pixels
	Vector2D centerPos = Vector2D(METERS_TO_PIXELS(centerPosInMeters.x), METERS_TO_PIXELS(centerPosInMeters.y));

	return centerPos;
}

void Player::Shoot()
{
	// Get mouse position in world coordinates
	b2Vec2 mousePos = {
		Engine::GetInstance().input->GetMousePosition().getX() - Engine::GetInstance().render->camera.x,
		Engine::GetInstance().input->GetMousePosition().getY() 
	};

	// Get the player's center position in world coordinates
	b2Vec2 centerPos = { GetCenterPosition().getX(), GetCenterPosition().getY() };
	
	// Calculate the direction vector from the player's center to the mouse position
	b2Vec2 direction = {
		mousePos.x - centerPos.x,
		mousePos.y - centerPos.y
	};

	// Normalize the direction vector to make it a unit vector
	float length = sqrt(direction.x * direction.x + direction.y * direction.y);
	if (length > 0.0f) { // Avoid division by zero
		direction.x /= length;
		direction.y /= length;
	}

	// Define the projectile spawn position with an offset in the normalized direction
	float offset = static_cast<float>(height); // Offset distance to avoid overlapping with the player
	b2Vec2 projectilePos = {
		centerPos.x + direction.x * offset,
		centerPos.y + direction.y * offset
	};

	// Adjust player orientation to face the direction of the projectile
	if (direction.x < 0) {
		animator->LookLeft();
	}
	else {
		animator->LookRight();
	}

	// Create and initialize the projectile with its position and direction in world space
	Projectile* projectile = (Projectile*)Engine::GetInstance().entityManager->CreateProjectile(projectilePos, direction, true);
	projectile->SetAnimation(0);
	projectile->SetGravity(0);
	projectile->SetCollisionType(0);

	// Reset the attack timer to manage firing rate
	attackShurikenTimer.Start();
}

bool Player::IsAttackingLeft() const
{
	return isAttackingLeft;
}

bool Player::IsAttackingRight() const
{
	return isAttackingRight;
}

void Player::SetAttackingLeft(bool isAttackingLeft_)
{
	isAttackingLeft = isAttackingLeft_;
}

void Player::SetAttackingRight(bool isAttackingRight_)
{
	isAttackingRight = isAttackingRight_;
}
