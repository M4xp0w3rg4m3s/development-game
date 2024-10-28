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


Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
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

	bodyBot = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), width*0.8, height*0.25, bodyType::DYNAMIC);

	body->CreateWeld(bodyBot, (float)PIXEL_TO_METERS((-height / 2)));

	body->body->SetFixedRotation(true);
	bodyBot->body->SetFixedRotation(true);

	b2MassData playerMass;
	playerMass.mass = 1.15f;
	playerMass.center = body->body->GetLocalCenter();
	body->body->SetMassData(&playerMass);

	body->body->GetFixtureList()[0].SetFriction(0);

	// Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	body->listener = this;
	bodyBot->listener = this;

	// Assign collider type
	body->ctype = ColliderType::PLAYER;
	bodyBot->ctype = ColliderType::GROUND_CHECK;

	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, body->body->GetLinearVelocity().y);

	isGrounded = bodyBot->activeCollisions != 0;
	
	if (!godMode) {
		if (state != PlayerState::DYING && state != PlayerState::DEAD)
		{
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				if (state == PlayerState::WOMBO || state == PlayerState::COMBO)
				{
					if (animator->GetPlayerDir() == RIGHT)
					{
						velocity.x = -0.2 * 16;
						animator->LookLeft();
						state = PlayerState::RUNNING;
					}
					else
					{
						velocity.x = -0.2 * 16;
						animator->LookLeft();
					}
				}
				else
				{
					velocity.x = -0.2 * 16;
					animator->LookLeft();
					state = PlayerState::RUNNING;
				}
			}
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				if (state == PlayerState::WOMBO || state == PlayerState::COMBO)
				{
					if (animator->GetPlayerDir() == LEFT)
					{
						velocity.x = 0.2 * 16;
						animator->LookRight();
						state = PlayerState::RUNNING;
					}
					else {
						velocity.x = 0.2 * 16;
						animator->LookRight();
					}
				}
				else
				{
					velocity.x = 0.2 * 16;
					animator->LookRight();
					state = PlayerState::RUNNING;
				}
			}
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
				if (isGrounded) {
					velocity.y = 0;
					body->body->SetLinearVelocity(velocity);
					body->body->ApplyLinearImpulseToCenter(b2Vec2{ 0, -7.5}, true);
					bodyBot->body->ApplyLinearImpulseToCenter(b2Vec2{ 0, -7.5}, true);
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
		}
	}


	body->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = body->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - 64 / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - 64 / 2);

	if (state != PlayerState::DYING && state != PlayerState::DEAD){
		if (state == PlayerState::WOMBO ) {
			if (animator->isAnimFinished() && animator->GetAnimation() != 5 && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && attackReactionTimer.ReadMSec() < reactionTimeMs)
			{
				animator->SetAnimation(5);
				animator->SetLoop(false);
				state = PlayerState::COMBO;
				attackCooldownTimer.Start();
			}
			else if(animator->isAnimFinished() && animator->GetAnimation() != 5 && attackReactionTimer.ReadMSec() > reactionTimeMs){
				animator->SetAnimation(1);
				state = PlayerState::IDLE;
				attackCooldownTimer.Start();
			}
		}
		else if (state == PlayerState::COMBO) {
			if (animator->GetCurrentFrame_int() == 3) {
				SDL_Delay(100);
				state = PlayerState::IDLE;
			}
		}
	
		else if (state != PlayerState::WOMBO && state != PlayerState::COMBO)
		{
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && attackCooldownTimer.ReadMSec() > attackCooldown)
			{
				if (animator->GetAnimation() != 4) {
					animator->SetAnimation(4);
					animator->SetLoop(false);
					state = PlayerState::WOMBO;
					attackReactionTimer.Start();
				}
			}
			else{
				if (isGrounded) {
					if (velocity.x == 0) {
						if (animator->GetAnimation() != 1)
						{
							animator->SetAnimation(1);
							state = PlayerState::IDLE;
						}
					}
					else {
						if (animator->GetAnimation() != 0)
						{
							animator->SetAnimation(0);
							state = PlayerState::RUNNING;
						}
					}
				}
				else {
					if (animator->GetAnimation() != 2)
					{
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
	switch (physB->ctype)
	{
	case ColliderType::KILL:
		LOG("Collision KILL");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		//Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
		//Engine::GetInstance().physics.get()->DeletePhysBody(physB);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
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
	position = Vector2D(192, 384);
	Engine::GetInstance().scene->CameraReset();
	state = PlayerState::IDLE;
	Enable();
}

void Player::ResetPlayer(int level)
{
	Disable();
	Engine::GetInstance().physics->DeletePhysBody(body);
	Engine::GetInstance().physics->DeletePhysBody(bodyBot);
	if (level == 1) {
		position = Vector2D(192, 384);
	}
	if (level == 2) {
		position = Vector2D(192, 320);
	}
	Engine::GetInstance().scene->CameraReset();
	state = PlayerState::IDLE;
	Enable();
}