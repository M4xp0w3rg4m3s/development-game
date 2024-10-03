#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	//Initialize Player parameters
	position = Vector2D(0, 0);
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set( position.getX(), position.getY());
	body = Engine::GetInstance().scene.get()->world->CreateBody(&bodyDef);

	b2PolygonShape rectangle;
	rectangle.SetAsBox(PIXEL_TO_METERS(10 / 2.0f), PIXEL_TO_METERS(10 / 2.0f));

	b2PolygonShape boxShape = rectangle;

	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 1.0f;
	boxFixtureDef.friction = 0.3f;
	body->CreateFixture(&boxFixtureDef);

	return true;
}

bool Player::Start() {

	texture = Engine::GetInstance().textures.get()->Load(textureName.c_str());
	return true;
}

bool Player::Update(float dt)
{
	b2Vec2 velocityDir{ 0, body->GetLinearVelocity().y };

	//Render the player texture and modify the position of the player using WSAD keys and render the texture

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		velocityDir.x -= speed*(dt/1000);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		velocityDir.x += speed*(dt/1000);

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		velocityDir.y = 0;
		body->SetLinearVelocity(velocityDir);
		body->ApplyForceToCenter(b2Vec2{ 0, -12 }, true);
	}

	body->SetLinearVelocity(velocityDir);

	position.setX(body->GetPosition().x);
	position.setY(body->GetPosition().y);

	printf("ola, %f\n", body->GetLinearVelocity().y);
	Engine::GetInstance().render.get()->DrawTexture(texture, METERS_TO_PIXELS(position.getX()), METERS_TO_PIXELS(position.getY()));
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}