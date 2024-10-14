#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"


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

	texture = Engine::GetInstance().textures.get()->Load(textureName.c_str());

	animator = new Sprite(texture);
	animator->SetNumberAnimations(2);
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

	//JUMP UP
	animator->AddKeyFrame(2, { 0 * 64,2 * 64,64,64 });
	animator->AddKeyFrame(2, { 1 * 64,2 * 64,64,64 });
	animator->AddKeyFrame(2, { 2 * 64,2 * 64,64,64 });
	animator->AddKeyFrame(2, { 3 * 64,2 * 64,64,64 });
	animator->SetAnimationDelay(2, 100);

	//FALL
	animator->AddKeyFrame(3, { 0 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(3, { 1 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(3, { 2 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(3, { 3 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(3, { 4 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(3, { 5 * 64,3 * 64,64,64 });
	animator->AddKeyFrame(3, { 6 * 64,3 * 64,64,64 });
	animator->SetAnimationDelay(3, 100);

	//LAND
	animator->AddKeyFrame(4, { 1 * 64,4 * 64,64,64 });
	animator->AddKeyFrame(4, { 2 * 64,4 * 64,64,64 });
	animator->AddKeyFrame(4, { 3 * 64,4 * 64,64,64 });
	animator->SetAnimationDelay(4, 100);

	//DIE
	animator->AddKeyFrame(5, { 0 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(5, { 1 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(5, { 2 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(5, { 3 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(5, { 4 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(5, { 5 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(5, { 6 * 64,5 * 64,64,64 });
	animator->AddKeyFrame(5, { 7 * 64,5 * 64,64,64 });
	animator->SetAnimationDelay(5, 100);

	//COMBO PART1
	animator->AddKeyFrame(6, { 0 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(6, { 1 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(6, { 2 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(6, { 3 * 64,6 * 64,64,64 });
	animator->SetAnimationDelay(6, 100);

	//COMBO PART2
	animator->AddKeyFrame(7, { 4 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(7, { 5 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(7, { 6 * 64,6 * 64,64,64 });
	animator->AddKeyFrame(7, { 7 * 64,6 * 64,64,64 });
	animator->SetAnimationDelay(7, 100);

	animator->SetAnimation(0);



	// Add physics to the player - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);

	body = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), 20, 45, bodyType::DYNAMIC);

	b2MassData playerMass;
	playerMass.mass = 1.15f;
	playerMass.center = body->body->GetLocalCenter();
	body->body->SetMassData(&playerMass);

	body->body->GetFixtureList()[0].SetFriction(0);

	// Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	body->listener = this;

	// Assign collider type
	body->ctype = ColliderType::PLAYER;

	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, body->body->GetLinearVelocity().y);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2 * dt;
		animator->LookLeft();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2 * dt;
		animator->LookRight();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		if (canJump) {
			velocity.y = 0;
			body->body->SetLinearVelocity(velocity);
			body->body->ApplyForceToCenter(b2Vec2{ 0, (float)METERS_TO_PIXELS(-9) }, true);
			canJump = false;

			//PlayerState = JUMPING
		}
	}

	body->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = body->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - 64 / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - 64 / 2);

	//Grounded
	if (velocity.y < 0) {
		if (animator->GetAnimation() != 2)
			animator->SetAnimation(2);
	}
	else if (velocity.y > 0) {
		if (animator->GetAnimation() != 3)
			animator->SetAnimation(3);
	}
	else if (velocity.x == 0 && velocity.y == 0) {
		if (animator->GetAnimation() != 1)
		{
			animator->SetAnimation(1);
		}
	}
	else if(velocity.x != 0 && velocity.y == 0) {
		if (animator->GetAnimation() != 0)
		{
			animator->SetAnimation(0);
			
		}
	}
	

	animator->Update();
	animator->Draw((int)position.getX(), (int)position.getY());
	//Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		canJump = true;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}