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

}

bool Player::Awake() {

	//Initialize Player parameters
	position = Vector2D(128, 0);
	return true;
}

bool Player::Start() {

	texture = Engine::GetInstance().textures.get()->Load(textureName.c_str());

	// Add physics to the player - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	body = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	// Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	body->listener = this;

	// Assign collider type
	body->ctype = ColliderType::PLAYER;

	return true;
}

bool Player::Update(float dt)
{
	//b2Vec2 velocityDir{ 0, body->GetLinearVelocity().y };

	////Render the player texture and modify the position of the player using WSAD keys and render the texture

	//if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	//	velocityDir.x -= speed*(dt/1000);

	//if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	//	velocityDir.x += speed*(dt/1000);

	//if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
	//	velocityDir.y = 0;
	//	body->SetLinearVelocity(velocityDir);
	//	body->ApplyForceToCenter(b2Vec2{ 0, -12 }, true);
	//}

	//body->SetLinearVelocity(velocityDir);

	//position.setX(body->GetPosition().x);
	//position.setY(body->GetPosition().y);

	//Engine::GetInstance().render.get()->DrawTexture(texture, METERS_TO_PIXELS(position.getX()), METERS_TO_PIXELS(position.getY()));

	b2Vec2 velocity = b2Vec2(0, body->body->GetLinearVelocity().y);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2 * dt;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2 * dt;
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		if (canJump) {
			velocity.y = 0;
			body->body->SetLinearVelocity(velocity);
			body->body->ApplyForceToCenter(b2Vec2{ 0, (float)METERS_TO_PIXELS(-9) }, true);
			canJump = false;
		}
	}

	body->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = body->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());

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