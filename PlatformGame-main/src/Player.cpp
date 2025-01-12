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

	audioPlayerStepsGrassId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/stepGrass.wav"); //AUDIO STEPS
	audioPlayerStepsRockId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/stepRock.wav");
	audioPlayerStepsId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/steps3.wav"); //AUDIO STEPS
	audioPlayerSwordId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/swordPlayer.wav"); //AUDIO SWORD
	audioShurikenShootId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/shurikenShoot.wav");
	audioPlayerSwordSwingId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/swordSwing.wav");
	audioPlayerHurtId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/playerHurt.wav");
	audioPlayerDieId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/playerDie.wav");
	audioHitEnemyId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/hitEnemy.wav");
	audioFallToWaterId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/fallToWater.wav");
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
	bodyBot = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), (int)(width * 0.8), (int)(height * 0.25), bodyType::DYNAMIC);
	bodyAttackLeft = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), (int)(width * 1.5), (int)(height * 1.5), bodyType::DYNAMIC);
	bodyAttackRight = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), (int)(width * 1.5), (int)(height * 1.5), bodyType::DYNAMIC);

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
	
	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, body->body->GetLinearVelocity().y);

	bool wasGrounded = isGrounded;

	isGrounded = bodyBot->activeCollisions != 0;

	int currentLevel = Engine::GetInstance().scene.get()->GetCurrentLevel();	

	if (!godMode) {
		if (state != PlayerState::DYING && state != PlayerState::DEAD)
		{
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocity.x = (float)-0.2 * 16;
				animator->LookLeft();
				if ((state == PlayerState::WOMBO || state == PlayerState::COMBO) && animator->GetPlayerDir() == RIGHT)
				{
					state = PlayerState::RUNNING;
				}
				
			}
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocity.x = (float)0.2 * 16;
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
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_REPEAT) {
		
			state = PlayerState::DYING;
		}
	}
	else {
		body->body->SetGravityScale(0);
		bodyBot->body->SetGravityScale(0);
		bodyAttackLeft->body->SetGravityScale(0);
		bodyAttackRight->body->SetGravityScale(0);

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = (float)-0.3 * 16;
			animator->LookLeft();
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = (float)0.3 * 16;
			animator->LookRight();
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			velocity.y = (float)-0.3 * 16;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			velocity.y = (float)0.3 * 16;
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
	position.setX((float)METERS_TO_PIXELS(pbodyPos.p.x) - 64 / 2);
	position.setY((float)METERS_TO_PIXELS(pbodyPos.p.y) - 64 / 2);

	//Shuriken Attack
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_DOWN && attackShurikenTimer.ReadMSec() > attackShurikenTime && shurikenEnabled) {
		Shoot();
	}
	if (enabledShurikenTimer.ReadMSec() > enabledShurikenTime) 
	{
		shurikenEnabled = false;
	}

	if (state != PlayerState::DYING && state != PlayerState::DEAD){
		if (state == PlayerState::WOMBO || state == PlayerState::COMBO)
		{
			if (enemyAttacked != nullptr)
			{
				if (animator->IsLookingLeft() && isAttackingLeft)
				{
					Engine::GetInstance().audio.get()->PlayFx(audioHitEnemyId); //Enemy hit Audio
					enemyAttacked->Disable();
					enemyAttacked = nullptr;
				}
				else if (animator->IsLookingRight() && isAttackingRight)
				{
					Engine::GetInstance().audio.get()->PlayFx(audioHitEnemyId); //Enemy hit Audio
					enemyAttacked->Disable();
					enemyAttacked = nullptr;
				}
			}
		}
		if (state == PlayerState::WOMBO) {
			if (animator->isAnimFinished() && animator->GetAnimation() != 5) {
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && attackReactionTimer.ReadMSec() < reactionTimeMs) {
					animator->SetAnimation(5);
					animator->SetLoop(false);
					
					state = PlayerState::COMBO;
				}
				else if (attackReactionTimer.ReadMSec() > reactionTimeMs) {
					isAttackingRight = false;
					isAttackingLeft = false;
					animator->SetAnimation(1);
					state = PlayerState::IDLE;
				}
				attackCooldownTimer.Start();
			}
		}
		else if (state == PlayerState::COMBO) {
			if (animator->GetCurrentFrame_int() == 3) {
				SDL_Delay(100);
				isAttackingRight = false;
				isAttackingLeft = false;
				state = PlayerState::IDLE;
			}
		}
		else {
			bool keyQPressed = Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_DOWN;
			bool canAttack = attackCooldownTimer.ReadMSec() > attackCooldown;

			if (keyQPressed && canAttack) {
				
				if (animator->GetAnimation() != 4) {
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
							isAttackingRight = false;
							isAttackingLeft = false;
							animator->SetAnimation(1);
							state = PlayerState::IDLE;
						}
					}
					else {
						if (animator->GetAnimation() != 0) {
							isAttackingRight = false;
							isAttackingLeft = false;
							animator->SetAnimation(0);
							state = PlayerState::RUNNING;
						}
					}
				}
				else {
					if (animator->GetAnimation() != 2) {
						isAttackingRight = false;
						isAttackingLeft = false;
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
		Engine::GetInstance().audio.get()->PlayFx(audioPlayerDieId); // Audio Die
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

	//Jump SoundFX
	if (isGrounded && !wasGrounded) {
		if (position.getX() < 1125 && currentLevel == 1) // Cave Level 1
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId);
		}
		else if (6460 > position.getX() && position.getX() > 6170 && currentLevel == 1) // House Level 1
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId);
		}
		else if (510 > position.getX() && position.getX() > 225 && currentLevel == 2) // Initial house Level 2
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId);
		}
		else if (3935 > position.getX() && position.getX() > 225 && currentLevel == 2) // End house Level 2
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId);
		}
		else
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsGrassId);
		}
	}

	//Walking SoundFX
	if (state == PlayerState::RUNNING && (animator->GetAnimation() == 0) && (animator->GetCurrentFrame_int() == 2 && animator->GetLastFrame_int() != 2 || animator->GetCurrentFrame_int() == 7 && animator->GetLastFrame_int() != 7)) {
		if (position.getX() < 1125 && currentLevel == 1) // Cave Level 1
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId); //player steps audio updates every step
		}
		/* steps rock beneath boulder */
		else if (4064 > position.getX() && position.getX() > 3712  && currentLevel == 1 && position.getY() > 360) // Boulder Level 1
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId); 
		}
		else if (6460 > position.getX() && position.getX() > 6170 && currentLevel == 1) // House Level 1
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId); 
		}
		else if (510 > position.getX() && position.getX() > 225 && currentLevel == 2) // Initial house Level 2
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId); 
		}
		else if (4225 > position.getX() && position.getX() > 3935 && currentLevel == 2) // End house Level 2
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsId); 
		}
		else
		{
			Engine::GetInstance().audio.get()->PlayFx(audioPlayerStepsGrassId);	
		}
	}
	//Combo1 SoundFX
	if (state == PlayerState::WOMBO && (animator->GetAnimation() == 4) && (animator->GetCurrentFrame_int() == 0 && animator->GetLastFrame_int() != 0)){
		printf("%d - %d\n", animator->GetCurrentFrame_int(), animator->GetLastFrame_int());
		Engine::GetInstance().audio.get()->PlayFx(audioPlayerSwordId);	//player sword play
		Engine::GetInstance().audio.get()->PlayFx(audioPlayerSwordSwingId); 

	}
	//Combo2 SoundFX
	if (state == PlayerState::COMBO && (animator->GetAnimation() == 5) && (animator->GetCurrentFrame_int() == 0 && animator->GetLastFrame_int() != 0)) {
		printf("%d - %d\n", animator->GetCurrentFrame_int(), animator->GetLastFrame_int());
		Engine::GetInstance().audio.get()->PlayFx(audioPlayerSwordId);	//player sword play
		Engine::GetInstance().audio.get()->PlayFx(audioPlayerSwordSwingId); 
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
	//Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (physA->ctype == ColliderType::GROUND_CHECK) {
		if (physB->ctype == ColliderType::KILL) {
			Engine::GetInstance().audio.get()->PlayFx(audioFallToWaterId); // Audio Water
			KillPlayer();
		}
	}
	if (physA->ctype == ColliderType::PLAYER_ATTACK_LEFT && physB->ctype == ColliderType::ENEMY) {
		isAttackingLeft = true;
		enemyAttacked = physB->listener;
	}
	else if (physA->ctype == ColliderType::PLAYER_ATTACK_RIGHT && physB->ctype == ColliderType::ENEMY) {
		isAttackingRight = true;
		enemyAttacked = physB->listener;
	}
	if (physA->ctype != ColliderType::PLAYER_ATTACK_LEFT && physA->ctype != ColliderType::PLAYER_ATTACK_RIGHT){
		switch (physB->ctype)
		{
		case ColliderType::KILL:
			LOG("Collision KILL");

			if (godMode) break;

			break;
		case ColliderType::ITEM:
			LOG("Collision ITEM");
			if (!physB->listener) {
				break;
			}
			if (physB->listener->name == "shuriken")
			{
				//Sound?
				shurikenEnabled = true;
				enabledShurikenTimer.Start();
				physB->listener->Disable();
			}
			else if (physB->listener->name == "ignis")
			{
				//Sound?
				currentIgnis++;
				physB->listener->Disable();
			}
			else if (physB->listener->name == "health")
			{
				//Sound?
				if (lives < 3)
				{
					lives++;
				}
				
				physB->listener->Disable();
			}
			else if (physB->listener->name == "coin")
			{
				Engine::GetInstance().audio->PlayFx(pickCoinFxId);
			}
			else {}
			break;
		case ColliderType::ENEMY:
			LOG("Collision UNKNOWN");

			if (godMode) break;

			body->body->ApplyLinearImpulseToCenter({ 0,-1 }, true);

			if (hitTimer.ReadMSec() > hitTime)
			{
				lives--;
				if (lives <= 0)
				{
					KillPlayer();
				}
				if (state != PlayerState::DYING && state != PlayerState::DEAD)
				{
					Engine::GetInstance().audio.get()->PlayFx(audioPlayerHurtId); // Audio Hurt
				}
				hitTimer.Start();
			}
			break;
		case ColliderType::PROJECTILE_ENEMY:
			LOG("Collision Projectile");

			if (godMode) break;

			body->body->ApplyLinearImpulseToCenter({ 0,-1 }, true);
			
			if (hitTimer.ReadMSec() > hitTime)
			{
				lives--;
				if (lives <= 0)
				{
					KillPlayer();
				}
				if (state != PlayerState::DYING && state != PlayerState::DEAD)
				{
					Engine::GetInstance().audio.get()->PlayFx(audioPlayerHurtId); // Audio Hurt
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
	if (!godMode) {
		state = PlayerState::DYING;
	}
}

void Player::ResetPlayer()
{
	Disable();
	Engine::GetInstance().physics->DeletePhysBody(body);
	Engine::GetInstance().physics->DeletePhysBody(bodyBot);
	Engine::GetInstance().physics->DeletePhysBody(bodyAttackLeft);
	Engine::GetInstance().physics->DeletePhysBody(bodyAttackRight);
	state = PlayerState::IDLE;
	lives = 3;
	Enable();
	Engine::GetInstance().scene->LoadState();
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
	if (level == 3) {
		position = Vector2D(192, 300);
	}
	state = PlayerState::IDLE;
	lives = 3;
	Enable();
}

void Player::SetPosition(Vector2D pos) {
	//pos.setX(pos.getX() + texW / 2);
	//pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	bodyBot->body->SetTransform(bodyPos, 0);
	bodyAttackLeft->body->SetTransform(bodyPos, 0);
	bodyAttackRight->body->SetTransform(bodyPos, 0);
	body->body->SetTransform(bodyPos, 0);
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = body->body->GetTransform().p;
	Vector2D pos = Vector2D((float)METERS_TO_PIXELS(bodyPos.x), (float)METERS_TO_PIXELS(bodyPos.y));
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
	Vector2D centerPos = Vector2D((float)METERS_TO_PIXELS(centerPosInMeters.x), (float)METERS_TO_PIXELS(centerPosInMeters.y));

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

	Engine::GetInstance().audio.get()->PlayFx(audioShurikenShootId); //Audio Shuriken

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

int Player::GetPlayerLives() const
{
	if (state != PlayerState::DYING && state != PlayerState::DEAD) {
		return lives;
	}
	return 0;
}
void Player::SetPlayerLives(int givenLives)
{
	lives = givenLives;
}

int Player::GetPlayerIgnis() const
{
	return currentIgnis;
}

void Player::SetPlayerIgnis(int givenIgnis)
{
	currentIgnis = givenIgnis;
}

double Player::GetAttackTimer() const
{
	return attackCooldownTimer.ReadMSec();
}

int Player::GetAttackTime() const
{
	return attackCooldown;
}

double Player::GetShurikenTimer() const
{
	return attackShurikenTimer.ReadMSec();
}

int Player::GetShurikenTime() const
{
	return attackShurikenTime;
}

int Player::IsShurikenEnabled() const
{
	return shurikenEnabled;
}

PlayerState Player::GetPlayerState() const
{
	return state;
}
